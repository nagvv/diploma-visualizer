#include "painter.h"
#include "common.h"

#include <QPen>
#include <QPainter>

Painter::Painter(QWidget *parent) : QWidget(parent)
{
    d.isExist=false;
}

void Painter::paint(struct data td)
{
    d=td;
}

void Painter::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this); // Создаём объект отрисовщика
    // Устанавливаем кисть абриса
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap));
    if(d.isExist)
    {
        for(int i=0; i < d.N; ++i)
            painter.drawEllipse(static_cast<int>(d.bots->posX), static_cast<int>(d.bots->posY), static_cast<int>(d.bots[i].radius*2 + 0.5f), static_cast<int>(d.bots[i].radius*2 + 0.5f));
    }
}
