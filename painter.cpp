#include "painter.h"
#include "common.h"

#include <QPen>
#include <QPainter>
#include <cmath>
#include <qtgifimage/src/gifimage/qgifimage.h>

Painter::Painter(QWidget *parent) : QWidget(parent)
{
    viewHeight = 150.f;
    viewX = 0.f;
    viewY = 0.f;
    showCenter = true;
    showLookAt = false;
    showBestPos = false;
    showTraces = false;
    recordTraces = true;
    colorRobots = false;

    maxSkip = 5;
    itSkip = 0;
}

void Painter::paint(shared_ptr<vector<bot>> bots)
{
    this->bots = bots;
    if(this->bots && recordTraces)
    {
        if(itSkip < maxSkip)
        {
            itSkip++;
            return;
        }
        itSkip = 0;
        traces.emplace_back();
        for(size_t i = 0; i < bots->size(); ++i)
        {
            QLineF ql;
            bool p1set = false;
            if( traces.size() > 1 )
                if( i < traces[traces.size() - 2].size() )
                {
                    p1set = true;
                    ql.setP1(traces[traces.size()-2][i].first.p2());
                }
            ql.setP2(QPointF((*bots)[i].posX, (*bots)[i].posY));

            if(!p1set)
                ql.setP1(ql.p2());
            float redcv = (*bots)[i].v/150;
            if(redcv > 1)
                redcv = 1.;
            if(redcv < 0)//because noises
                redcv = 0.;
            traces.back().push_back(make_pair(ql, QColor::fromRgbF(redcv, 1. - redcv, 0)));
        }
    }
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

void Painter::setWalls(const vector<QLineF> &walls)
{
    this->walls = walls;
}

void Painter::resetTraces()
{
    traces.clear();
}

void Painter::makeGif(const vector<shared_ptr<vector<bot> > > &data)
{
    shared_ptr<vector<bot>> save = bots;//saving current frame
    resetTraces();

    QGifImage gif;
    gif.setDefaultDelay(1);

    QImage image(this->width(), this->height(), QImage::Format_RGB32);
    for (unsigned int i = 0; i < data.size(); i+=2) //draw every second frame
    {
        paint(data[i]);
        image.fill(QColor(Qt::white));
        QPainter painter(&image);
        float size = this->height() / viewHeight;
        float viewWidth = viewHeight * this->width() / this->height();

        if(showTraces)
            drawTraces(painter, viewWidth, size);

        drawBots(painter, viewWidth, size);

        drawWalls(painter, viewWidth, size);

        if(showCenter)//draw center mark
            drawMark(painter, viewWidth, size);

        gif.addFrame(image);
    }
    gif.save("outgif.gif");

    resetTraces();
    paint(save);//returning to saved frame
}

void Painter::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this); // Создаём объект отрисовщика
    // Устанавливаем кисть абриса
    painter.setPen(Qt::black);

    float size = this->height() / viewHeight;
    float viewWidth = viewHeight * this->width() / this->height();

    if(showTraces)
        drawTraces(painter, viewWidth, size);

    drawBots(painter, viewWidth, size);

    drawWalls(painter, viewWidth, size);

    if(showCenter)//draw center mark
        drawMark(painter, viewWidth, size);
}

void Painter::drawTraces(QPainter &painter, float viewWidth, float size)
{
    for(auto &frame : traces)
        for(auto &t: frame)
        {
            QLineF trw = QLineF((t.first.x1() - viewX + viewWidth/2)*size, (t.first.y1() - viewY + viewHeight/2)*size,
                              (t.first.x2() - viewX + viewWidth/2)*size, (t.first.y2() - viewY + viewHeight/2)*size);
            painter.setPen(t.second);
            painter.drawLine(trw);
        }
    painter.setPen(Qt::black);
}

void Painter::drawBots(QPainter &painter, float viewWidth, float size)
{
    if(bots)
        for(auto &b: *bots)
        {
            float px = (b.posX - viewX + viewWidth/2)*size;
            float py = (b.posY - viewY + viewHeight/2)*size;
            float sz = 2*b.radius * size;
            if(colorRobots)
            {
                float redcv = b.v/150;
                if(redcv > 1)
                    redcv = 1.;
                if(redcv < 0)//because noises
                    redcv = 0.;
                painter.setPen(QColor::fromRgbF(redcv, 1 - redcv, 0));
            }
            painter.drawEllipse(px - sz/2, py - sz/2, sz, sz);

            float dx = b.dirX * size;
            float dy = b.dirY * size;
            painter.drawLine(px, py, px + dx, py + dy);
            //draw look at points
            if(showLookAt)
            {
                float lax = b.laX * size;
                float lay = b.laY * size;
                painter.setPen(QColor::fromRgb(0, 200, 0));
                painter.drawLine(px, py, px + lax, py + lay);
            }
            if(showBestPos)
            {
                float bx = b.bX * size;
                float by = b.bY * size;
                painter.setPen(QColor::fromRgb(200, 0, 0));
                painter.drawLine(px, py, px + bx, py + by);
            }
            painter.setPen(Qt::black);
        }
}

void Painter::drawWalls(QPainter &painter, float viewWidth, float size)
{
    for(auto &w: walls)
    {
        QLineF trw = QLineF((w.x1() - viewX + viewWidth/2)*size, (w.y1() - viewY + viewHeight/2)*size,
                          (w.x2() - viewX + viewWidth/2)*size, (w.y2() - viewY + viewHeight/2)*size);
        painter.drawLine(trw);
    }
}

void Painter::drawMark(QPainter &painter, float viewWidth, float size)
{
    int ls = 10;
    QLineF l1(-ls, 0, ls, 0), l2(0, -ls, 0, ls);
    QPointF trp( this->width()/2 - viewX*size, this->height()/2 - viewY*size );

    l1.translate( trp );
    l2.translate( trp );

    painter.drawLine(l1);
    painter.drawLine(l2);
}
