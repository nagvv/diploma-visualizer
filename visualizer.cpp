#include "visualizer.h"
#include "ui_visualizer.h"
#include "common.h"
#include <cmath>
#include <QFileInfo>

Visualizer::Visualizer(int t_id, QWidget *parent) : QWidget(parent, Qt::Window), ui(new Ui::Visualizer),
                                                    id(t_id), frame(0), pause(false)
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

    ui->painter->setWalls(ll.getWalls());

    repainter = new QTimer(this);
    connect(repainter, SIGNAL(timeout()), this, SLOT(repaint()));
    repainter->start(10);

    updater = new QTimer(this);
    connect(updater, SIGNAL(timeout()), this, SLOT(vUpdate()));
    updater->start(64);

    log(QString("Visualizer ") + QString::number(id) + QString("successfully constructed."));
}

Visualizer::~Visualizer()
{
    disconnect(repainter, SIGNAL(timeout()), this, SLOT(repaint()));
    disconnect(updater, SIGNAL(timeout()), this, SLOT(vUpdate()));
    delete ui;
    delete repainter;
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
    if(ll.isOkay())
    {
        if(frame >= ll.getFrameNum())
            frame=0;
        if(!pause)
        {
            ui->painter->paint(ll.getData()[frame]);
            frame++;
        }
    }

    if(common.isRunning() && frame == ll.getFrameNum() - 1)
        ll.read(ui->chooseBox->currentText());
}

void Visualizer::repaint()
{
    QString text;
    text += QString("zoom: ") + QString::number(100/ui->painter->getViewHeight());
    text += QString(", x: ") + QString::number(ui->painter->getViewX());
    text += QString(", y: ") + QString::number(ui->painter->getViewY());
    text += QString(", speed:") + QString::number(64./updater->interval());
    text += QString(", frame:") + QString::number(frame + 1) + "/" + QString::number(ll.getFrameNum());
    if(pause)
        text+=", paused";
    ui->statusBar->setText(text);
    QWidget::repaint();
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
    int current = updater->interval();
    if( current > 1 )
        updater->setInterval(current/2);
}

void Visualizer::on_decSpeedButton_clicked()
{
    int current = updater->interval();
    if( current < 512 )
        updater->setInterval(current*2);
}

void Visualizer::on_resetSpeedButton_clicked()
{
    updater->setInterval(64);
}

void Visualizer::on_restartButton_clicked()
{
    frame=0;
}

void Visualizer::on_chooseBox_currentIndexChanged(const QString &file)
{
    log("Visualizer: selected file: " + file);
    ll.read(ui->chooseBox->currentText());
    if(ll.isOkay())
        log("visualizer: logloader is okay");
    frame = 0;
}
