#ifndef PAINTER_H
#define PAINTER_H

#include <QWidget>
#include <vector>
#include "logloader.h"

using std::vector;

class Painter : public QWidget
{
    Q_OBJECT
private:
    vector<bot> bots;
    vector<QLine> walls;
    float viewHeight;
    float viewX, viewY;
protected:
    void paintEvent(QPaintEvent *event);

public:
    explicit Painter(QWidget *parent = nullptr);
    void paint(const vector<bot> &bots);

    void setViewPos(float x, float y);//Set the view center to (x, y).
    void moveViewPos(float mx, float my);//Moves the view center by (mx, my).

    void setZoom(float z);
    void addZoom(float az);

    inline float getViewHeight() { return viewHeight; }
    inline float getViewX() { return viewX; }
    inline float getViewY() { return viewY; }

    void setWalls(const vector<QLine>&);
signals:

public slots:
};

#endif // PAINTER_H
