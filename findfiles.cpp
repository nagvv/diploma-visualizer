#include "findfiles.h"
#include "ui_findfiles.h"
#include "mainwindow.h"
#include "common.h"

findFiles::findFiles(QWidget *parent) : QDialog(parent), ui(new Ui::findFiles)
{
    ui->setupUi(this);
    ui->execPath->setText(common.getExecPath());
    ui->configPath->setText(common.getConfigPath());
}

findFiles::~findFiles()
{
    delete ui;
}

void findFiles::on_execBrowse_clicked()
{
    QString s = QFileDialog::getOpenFileName(this, "Open exec");
    if(!s.isEmpty())
        ui->execPath->setText(s);
}

void findFiles::on_configBrowse_clicked()
{
    QString s = QFileDialog::getOpenFileName(this, "Open config");
    if(!s.isEmpty())
        ui->configPath->setText(s);
}

void findFiles::on_buttonBox_accepted()
{
    common.setExecPath(ui->execPath->text());
    common.setConfigPath(ui->configPath->text());
    dynamic_cast<MainWindow*>(parentWidget())->update();
}
