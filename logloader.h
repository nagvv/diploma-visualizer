#ifndef LOGLOADER_H
#define LOGLOADER_H
#include <vector>
#include <QString>
#include <QObject>
#include <QFile>
#include <QFileSystemWatcher>
#include <memory>

using std::vector;
using std::shared_ptr;
using std::make_shared;

struct bot
{
    bool alive = false;
    float   posX, posY, //position
            dirX, dirY, //direction
            radius, v,  //radius and best value
            bX, bY,        //best val position
            laX, laY;   //looking at point
};

struct grid
{
    int grid_size;
    int area_size;
    vector<float> cells;
};

class LogLoader: public QObject
{
    Q_OBJECT
private:
    vector< shared_ptr< vector<bot> > > data;
    vector< grid > grid_data;
    vector<QLineF> walls;
    size_t frameNum;
    std::unique_ptr<QFile> file;
    std::unique_ptr<QFile> grid_file;
    bool okay = false;
    QFileSystemWatcher watcher;
    QFileSystemWatcher obsWatcher;
    bool bObsChanged;

public:
    LogLoader();
    LogLoader(QString path);
    ~LogLoader();
    void read(QString path);
    void readObs(QString path);
    void readGrid(QString path);
    //update -- executed by signal from qfilesystemwatcher

    inline vector< shared_ptr< vector<bot> > > &getData() { return data; }
    inline const vector<QLineF> &getWalls() { return walls; }
    inline vector< grid > &getGrid() { return grid_data; }
    inline size_t getFrameNum() { return frameNum; }
    inline bool isOkay() { return okay; }

    bool isObsFileChanged();

public slots:
    void fileChanged(const QString &path);
    void obsFileChanged(const QString &path);
};

#endif // LOGLOADER_H
