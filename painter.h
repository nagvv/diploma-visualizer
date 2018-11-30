#ifndef PAINTER_H
#define PAINTER_H

#include <QWidget>
#include <vector>
#include <memory>
#include <utility>
#include "logloader.h"

using std::vector;
using std::shared_ptr;
using std::make_shared;
using std::pair;
using std::make_pair;

class Painter : public QWidget
{
    Q_OBJECT
private:
    shared_ptr<vector<bot>> bots;
    vector<QLineF> walls;
    vector<vector<pair<QLineF, QColor>>> traces;
    float viewHeight;
    float viewX, viewY;

    bool showCenter;
    bool showLookAt;
    bool showBestPos;
    bool showTraces;
    bool recordTraces;
    bool colorRobots;

    int maxSkip;
    int itSkip;
protected:
    void paintEvent(QPaintEvent *event);

public:
    explicit Painter(QWidget *parent = nullptr);
    void paint(shared_ptr<vector<bot>> bots);

    void setViewPos(float x, float y);//Set the view center to (x, y).
    void moveViewPos(float mx, float my);//Moves the view center by (mx, my).

    void setZoom(float z);
    void addZoom(float az);

    inline float getViewHeight() { return viewHeight; }
    inline float getViewX() { return viewX; }
    inline float getViewY() { return viewY; }

    inline void setShowCenter(bool sh) { showCenter = sh; }
    inline void setShowLookAt(bool sh) { showLookAt = sh; }
    inline void setShowBestPos(bool sh) { showBestPos = sh; }
    inline void setShowTraces(bool sh) { showTraces = sh; }
    inline void setColorRobots(bool sh) { colorRobots = sh; }

    void setWalls(const vector<QLineF>&);
    void resetTraces();
signals:

public slots:
};

#endif // PAINTER_H
