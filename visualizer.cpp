#include "visualizer.h"
#include "ui_visualizer.h"
#include "common.h"



Visualizer::Visualizer(int t_id, QWidget *parent) : QWidget(parent, Qt::Window), ui(new Ui::Visualizer), id(t_id)
{
    ui->setupUi(this);
    repainter = new QTimer(this);
    connect(repainter, SIGNAL(timeout()), this, SLOT(vUpdate()));
    repainter->start(20);
    d.bots = new bot[1];
    d.bots[0].radius = 5;
    d.bots[0].posX = 5;
    d.bots[0].posY = 50;
    d.N = 1;
    d.isExist=true;
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

void Visualizer::vUpdate()
{
    d.bots[0].posX++;
    ui->painter->paint(d);
    repaint();
}

