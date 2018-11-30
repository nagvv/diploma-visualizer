#include "visualizer.h"
#include "ui_visualizer.h"
#include "common.h"
#include <cmath>
#include <QFileDialog>
#include <QFileInfo>

Visualizer::Visualizer(int t_id, QWidget *parent) : QWidget(parent, Qt::Window), ui(new Ui::Visualizer),
                                                    id(t_id), frame(0), spcu(0), spto(64), pause(false)
{
    ui->setupUi(this);

    if( QFileInfo(common.getExecPath()).exists() )
        dir = QFileInfo(common.getExecPath()).dir();
    else
        dir = QDir::current();

    QStringList fileList = dir.entryList(QStringList("log_*_*"), QDir::Files);
    ui->chooseBox->addItems(fileList);
    if(ui->chooseBox->count() > 0)
        ui->chooseBox->setCurrentIndex(0);

    ll.readObs("obs1.txt");//default obstacles file name
    ui->painter->setWalls(ll.getWalls());

    updater = new QTimer(this);
    connect(updater, SIGNAL(timeout()), this, SLOT(vUpdate()));
    updater->start(16);

    log(QString("Visualizer ") + QString::number(id) + QString("successfully constructed."));
}

Visualizer::~Visualizer()
{
    disconnect(updater, SIGNAL(timeout()), this, SLOT(vUpdate()));
    delete ui;
    delete updater;
}

void Visualizer::closeEvent(QCloseEvent *event)
{
    log("visualizer: got closeEvent");
    common.removeVis(id);
    event->accept();
}

void Visualizer::vUpdate()
{
    float bv = INFINITY;
    if(ll.isOkay())
    {
        if(frame >= ll.getFrameNum())
        {
            frame=0;
            ui->painter->resetTraces();
        }

        if(ll.getData()[frame])
            for(auto &b: *ll.getData()[frame])
            {
                if(b.v < bv)
                    bv = b.v;
            }

        if(!pause)
        {
            ui->painter->paint(ll.getData()[frame]);
            spcu+=updater->interval();
            if(spcu >= spto)
            {
                frame+= spcu / spto;
                spcu = spcu % spto;
            }
        }
    }

    if(common.isRunning() && frame + 1 >= ll.getFrameNum())
    {
        ll.read(ui->chooseBox->currentText());
    }

    if(ll.isObsFileChanged())
        ui->painter->setWalls(ll.getWalls());

    QString text;
    text += QString("zoom: ") + QString::number(150/ui->painter->getViewHeight());
    text += QString(", x: ") + QString::number(ui->painter->getViewX());
    text += QString(", y: ") + QString::number(ui->painter->getViewY());
    text += QString(", speed:") + QString::number(64./spto);
    text += QString(", frame:") + QString::number(frame + 1) + "/" + QString::number(ll.getFrameNum());

    text += QString(", best:") + QString::number(bv);
    if(pause)
        text+=", paused";
    ui->statusBar->setText(text);
    if(mvUp)
        ui->painter->moveViewPos(0, -0.8 * ui->painter->getViewHeight()/100);
    if(mvLeft)
        ui->painter->moveViewPos(-0.8 * ui->painter->getViewHeight()/100, 0);
    if(mvDown)
        ui->painter->moveViewPos(0, 0.8 * ui->painter->getViewHeight()/100);
    if(mvRight)
        ui->painter->moveViewPos(0.8 * ui->painter->getViewHeight()/100, 0);
    if(mvZoomIn)
        ui->painter->addZoom(-0.04f);
    if(mvZoomOut)
        ui->painter->addZoom(0.04f);

    repaint();
}

void Visualizer::keyPressEvent(QKeyEvent *key)
{
    switch (key->key())
    {
    case Qt::Key_W:
        mvUp = true;
        break;
    case Qt::Key_A:
        mvLeft = true;
        break;
    case Qt::Key_S:
        mvDown = true;
        break;
    case Qt::Key_D:
        mvRight = true;
        break;
    case Qt::Key_E:
        mvZoomIn = true;
        break;
    case Qt::Key_Q:
        mvZoomOut = true;
        break;
    case Qt::Key_Space:
        pause = !pause;
        break;
    }
}

void Visualizer::keyReleaseEvent(QKeyEvent *key)
{
    switch (key->key())
    {
    case Qt::Key_W:
        mvUp = false;
        break;
    case Qt::Key_A:
        mvLeft = false;
        break;
    case Qt::Key_S:
        mvDown = false;
        break;
    case Qt::Key_D:
        mvRight = false;
        break;
    case Qt::Key_E:
        mvZoomIn = false;
        break;
    case Qt::Key_Q:
        mvZoomOut = false;
        break;
    }
}

void Visualizer::on_exitButton_clicked()
{
    log("visualizer: exit button clicked");
    this->close();
}

void Visualizer::on_centerButton_clicked()
{
    ui->painter->setViewPos(0, 0);
}

void Visualizer::on_leftButton_clicked()
{
    ui->painter->moveViewPos(-10 * ui->painter->getViewHeight()/100, 0);
}

void Visualizer::on_rightButton_clicked()
{
    ui->painter->moveViewPos(10 * ui->painter->getViewHeight()/100, 0);
}

void Visualizer::on_upButton_clicked()
{
    ui->painter->moveViewPos(0, -10 * ui->painter->getViewHeight()/100);
}

void Visualizer::on_downButton_clicked()
{
    ui->painter->moveViewPos(0, 10 * ui->painter->getViewHeight()/100);
}

void Visualizer::on_zoomInButton_clicked()
{
    ui->painter->addZoom(-0.2f);
}

void Visualizer::on_zoomOutButton_clicked()
{
    ui->painter->addZoom(0.2f);
}

void Visualizer::on_resetZoomButton_clicked()
{
    ui->painter->setZoom(0);
}

void Visualizer::on_resetButton_clicked()
{
    ui->painter->setViewPos(0, 0);
    ui->painter->setZoom(0);
}

void Visualizer::on_playPauseButton_clicked()
{
    pause = !pause;
}

void Visualizer::on_incSpeedButton_clicked()
{
    if( spto > 1 )
    {
        spto/=2;
        ui->decSpeedButton->setEnabled(true);
    }

    if(spto == 1)
        ui->incSpeedButton->setEnabled(false);
}

void Visualizer::on_decSpeedButton_clicked()
{
    if( spto < 512 )
    {
        spto*=2;
        ui->incSpeedButton->setEnabled(true);
    }

    if(spto == 512)
        ui->decSpeedButton->setEnabled(false);
}

void Visualizer::on_resetSpeedButton_clicked()
{
    spto = 64;
    ui->incSpeedButton->setEnabled(true);
    ui->decSpeedButton->setEnabled(true);
}

void Visualizer::on_restartButton_clicked()
{
    frame=0;
    ui->painter->resetTraces();
}

void Visualizer::on_chooseBox_currentIndexChanged(const QString &file)
{
    log("Visualizer: selected file: " + dir.path() + '/' + file);
    ll.read(dir.path() + '/' + file);
    if(ll.isOkay())
        log("visualizer: logloader is okay");
    frame = 0;
    ui->painter->resetTraces();
}

void Visualizer::on_chooseButton_clicked()
{
    QString s = QFileDialog::getExistingDirectory(this, "Choose Directory", dir.path());
    if(!s.isEmpty())
        dir = QDir(s);

    QStringList fileList = dir.entryList(QStringList("log_*_*"), QDir::Files);
    ui->chooseBox->clear();
    ui->chooseBox->addItems(fileList);
    if(ui->chooseBox->count() > 0)
        ui->chooseBox->setCurrentIndex(0);
}

void Visualizer::on_obsButton_clicked()
{
    QString s = QFileDialog::getOpenFileName(this, "Choose obstacles file", dir.path());
    if(!s.isEmpty())
    {
        ll.readObs(s);
        ui->painter->setWalls(ll.getWalls());
    }
}

void Visualizer::on_shCenterCheckBox_stateChanged(int set)
{
    ui->painter->setShowCenter(set);
}

void Visualizer::on_shLookAtCheckBox_stateChanged(int set)
{
    ui->painter->setShowLookAt(set);
}

void Visualizer::on_shBestPosCheckBox_stateChanged(int set)
{
    ui->painter->setShowBestPos(set);
}

void Visualizer::on_shTracesCheckBox_stateChanged(int set)
{
    ui->painter->setShowTraces(set);
}

void Visualizer::on_colorBotsChBox_stateChanged(int set)
{
    ui->painter->setColorRobots(set);
}
