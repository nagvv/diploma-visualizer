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
    grid current_grid;
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
    bool fillRobots;

    int maxSkip;
    int itSkip;

    float maxValue;
protected:
    void paintEvent(QPaintEvent *event);

public:
    explicit Painter(QWidget *parent = nullptr);
    void paint(shared_ptr<vector<bot>> bots, grid _grid);

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
    inline void setFillRobots(bool sh) { fillRobots = sh; }

    void setWalls(const vector<QLineF>&);
    void resetTraces();

    void makeGif(const vector< shared_ptr< vector<bot> > > &data, const vector< grid > & grid_data);

private:
    void drawTraces(QPainter &painter, float viewWidth, float size);
    void drawBots(QPainter &painter, float viewWidth, float size);
    void drawWalls(QPainter &painter, float viewWidth, float size);
    void drawMark(QPainter &painter, float viewWidth, float size);
    void drawGrid(QPainter &painter, float viewWidth, float size);

signals:

public slots:
};

#endif // PAINTER_H
