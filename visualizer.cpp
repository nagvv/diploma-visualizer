#include "visualizer.h"
#include "ui_visualizer.h"
#include "common.h"

Visualizer::Visualizer(int t_id, QWidget *parent) : QWidget(parent, Qt::Window), ui(new Ui::Visualizer), id(t_id)
{
    ui->setupUi(this);
}

Visualizer::~Visualizer()
{
    delete ui;
}

void Visualizer::closeEvent(QCloseEvent *event)
{
    log("visualizer: got closeEvent");
    common.removeVis(id);
    event->accept();
}
