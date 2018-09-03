#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "common.h"
#include "findfiles.h"
#include "visualizer.h"
#include <QFileInfo>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    update();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update()
{
    log("mainWindow: update");
    if(common.getExecPath().isEmpty())
    {
        ui->execCheck->setText("exec: Not found");
        ui->execCheck->setStyleSheet("background-color: red;");
        ui->startButton->setEnabled(false);
        log("execpath empty");
    }
    else if(!QFileInfo(common.getExecPath()).isExecutable())
    {
        ui->execCheck->setText("exec: Not executable");
        ui->execCheck->setStyleSheet("background-color: yellow;");
        ui->startButton->setEnabled(false);
        log("exec not executable");
    }
    else
    {
        ui->execCheck->setText("exec: Found");
        ui->execCheck->setStyleSheet("background-color: green;");
        ui->startButton->setEnabled(true);
        log("execpath filled");
    }

    if(common.getConfigPath().isEmpty())
    {
        ui->configCheck->setText("config: Not found");
        ui->configCheck->setStyleSheet("background-color: red;");
        ui->editButton->setEnabled(false);
        log("confpath empty");
    }
    else if(!QFileInfo(common.getConfigPath()).isFile())
    {
        ui->execCheck->setText("config: Not a file");
        ui->execCheck->setStyleSheet("background-color: red;");
        ui->editButton->setEnabled(false);
        log("exec not executable");
    }
    else
    {
        ui->configCheck->setText("config: Found");
        ui->configCheck->setStyleSheet("background-color: green;");
        ui->editButton->setEnabled(true);
        log("confpath filled");
    }
    qApp->processEvents();
}

void MainWindow::on_chooseBtn_clicked()
{
    log("mainWindow: chooseBtn clicked");
    findFiles f(this);
    f.exec();
}

void MainWindow::on_visBtn_clicked()
{
    log("mainWindow: visBtn clicked");
    common.addVis(this);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    log("mainWindow: closing");
    common.clearVises();
    common.stopExec();
    event->accept();
}

void MainWindow::on_isMPI_stateChanged(int arg1)
{
    log(QString("mainWindow: isMPIbox state changed to ") + QString::number(arg1));
    if(arg1)
        ui->pnumBox->setEnabled(true);
    else
        ui->pnumBox->setEnabled(false);

}

void MainWindow::on_startButton_clicked()
{
    log("mainWindow: startButton clicked");  
    common.runExec(this, ui->isMPI->isChecked(), ui->pnumBox->value());
}

void MainWindow::on_stopButton_clicked()
{
    log("mainWindow: stopButton clicked");
    common.stopExec();
}

void MainWindow::on_editButton_clicked()
{
    log("mainWindow: editButton clicked");
    QDesktopServices::openUrl(QUrl(common.getConfigPath()));
}

void MainWindow::pStarted()
{
    ui->runStatusLabel->setText("Started");
    //log("mainWindow: approved");
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
    ui->chooseBtn->setEnabled(false);
}

void MainWindow::pStateChanged(QProcess::ProcessState pState)
{
    ui->runStatusLabel->setText(QString("State changed to: %1").arg(pState));
    //log("mainWindow: approved");
}

void MainWindow::pFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    ui->runStatusLabel->setText(QString("Stopped with code: %1, and exitStatus: %2").arg(exitCode).arg(exitStatus));
    //log("mainWindow: approved");
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    ui->chooseBtn->setEnabled(true);
}

void MainWindow::pErrorOccured(QProcess::ProcessError error)
{
    QString t = QString("errorOccured: %1: ").arg(error);
    switch(error)
    {
    case 0:
        t+="The process failed to start. Either the invoked program is missing, or you may have insufficient permissions to invoke the program.";
        break;
    case 1:
        t+="The process crashed some time after starting successfully.";
        break;
    case 2:
        t+="The last waitFor...() function timed out. The state of QProcess is unchanged, and you can try calling waitFor...() again.";
        break;
    case 3:
        t+="An error occurred when attempting to read from the process. For example, the process may not be running.";
        break;
    case 4:
        t+="An error occurred when attempting to write to the process. For example, the process may not be running, or it may have closed its input channel.";
        break;
    case 5:
        t+="An unknown error occurred. ";
        break;
    }
    ui->runStatusLabel->setText(t);
    //log("mainWindow: approved");
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    ui->chooseBtn->setEnabled(true);
}


