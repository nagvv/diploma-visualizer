#ifndef LOGLOADER_H
#define LOGLOADER_H
#include <vector>
#include <QString>
#include <QObject>
#include <QFile>
#include <QFileSystemWatcher>
#include <memory>

using std::vector;

struct bot
{
    bool alive = false;
    float   posX, posY, //position
            dirX, dirY, //direction
            radius, v,  //radius and best value
            bX, bY,     //best val position
            clX, clY;   //looking at point
};

class LogLoader: public QObject
{
    Q_OBJECT
private:
    vector<vector<bot>> data;
    vector<QLine> walls;
    size_t botNum;
    size_t frameNum;
    std::unique_ptr<QFile> file;
    bool okay = false;
    QFileSystemWatcher watcher;
        //and other parameters

public:
    LogLoader();
    LogLoader(QString path);
    ~LogLoader();
    void read(QString path);
    //update -- executed by signal from qfilesystemwatcher

    inline const vector<vector<bot>> &getData() { return data; }
    inline const vector<QLine> &getWalls() { return walls; }
    inline size_t getBotNum() { return botNum; }
    inline size_t getFrameNum() { return frameNum; }
    inline bool isOkay() { return okay; }

public slots:
    void fileChanged(const QString &path);
};

#endif // LOGLOADER_H
