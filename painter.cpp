#include "painter.h"
#include "common.h"

#include <QPen>
#include <QPainter>
#include <cmath>

Painter::Painter(QWidget *parent) : QWidget(parent)
{
    viewHeight = 100.f;
    viewX = 0.f;
    viewY = 0.f;
}

void Painter::paint(const vector<bot> &bots)
{
    this->bots = bots;//uneffective!
}

void Painter::setViewPos(float x, float y)
{
    viewX = x;
    viewY = y;
}

void Painter::moveViewPos(float mx, float my)
{
    viewX += mx;
    viewY += my;
}

void Painter::setZoom(float z)
{
    viewHeight = 100 * std::exp2(z);
}

void Painter::addZoom(float az)
{
    viewHeight *= std::exp2(az);
}

void Painter::setWalls(const vector<QLine> &walls)
{
    this->walls = walls;
}

void Painter::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this); // Создаём объект отрисовщика
    // Устанавливаем кисть абриса
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap));

    float size = this->height() / viewHeight;
    float viewWidth = viewHeight * this->width() / this->height();

    //draw them!
    for(auto &b: bots)
    {
        float px = (b.posX - viewX + viewWidth/2)*size;
        float py = (b.posY - viewY + viewHeight/2)*size;
        float sz = 2*b.radius * size;
        painter.drawEllipse(px, py, sz, sz);
        float dx = b.dirX * size;
        float dy = b.dirY * size;
        painter.drawLine(px + sz/2, py + sz/2, px + sz/2 + dx, py + sz/2 + dy);
    }

    //draw walls
    for(auto &w: walls)
    {
        QLine trw = QLine((w.x1() - viewX + viewWidth/2)*size, (w.y1() - viewY + viewHeight/2)*size,
                          (w.x2() - viewX + viewWidth/2)*size, (w.y2() - viewY + viewHeight/2)*size);
        painter.drawLine(trw);
    }

    //draw center mark
    int ls = 10;
    QLine l1(-ls, 0, ls, 0), l2(0, -ls, 0, ls);
    QPoint trp( this->width()/2 - viewX*size, this->height()/2 - viewY*size );

    l1.translate( trp );
    l2.translate( trp );

    painter.drawLine(l1);
    painter.drawLine(l2);
}
