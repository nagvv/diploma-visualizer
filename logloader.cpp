#include "logloader.h"
#include <QTextStream>
#include "common.h"

LogLoader::LogLoader() : botNum(0), frameNum(0), bObsChanged(false)
{
    connect(&watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(fileChanged(const QString&)), Qt::QueuedConnection);
    connect(&obsWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(obsFileChanged(const QString&)), Qt::QueuedConnection);
}

LogLoader::LogLoader(QString path) : botNum(0), frameNum(0), bObsChanged(false)
{
    connect(&watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(fileChanged(const QString&)), Qt::QueuedConnection);
    connect(&obsWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(obsFileChanged(const QString&)), Qt::QueuedConnection);
    read(path);
}

LogLoader::~LogLoader()
{
    disconnect(&watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(fileChanged(const QString&)));
    disconnect(&obsWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(obsFileChanged(const QString&)));
}

void LogLoader::read(QString path)
{
    file = std::unique_ptr<QFile>(new QFile(path));
    log("LogLoader: opening " + path);
    okay = file->open(QIODevice::ReadOnly | QIODevice::Text);
    if(okay)
    {
        log("LogLoader: successfully opened");
        QTextStream in(file.get());
        botNum=0;
        frameNum=0;
        size_t bi = 0;
        data.clear();
        data.emplace_back(make_shared<vector<bot>>());
        while(!in.atEnd())
        {
            QStringList line = in.readLine().split(' ');
            if(line.size() == 1)
            {
                line = in.readLine().split(' ');
                if(line.size() == 1)
                {
                    if(botNum==0)
                    {
                        botNum=bi;
                        bi=0;
                    }
                    frameNum++;
                    data.emplace_back(make_shared<vector<bot>>());
                    continue;
                }
                else
                {
                    //okay = false;
                    //log("LogLoader: bad file _0");
                    break;
                }
            }
            else if(line.size() != 10)
            {
                //okay = false;
                //log("LogLoader: bad file _1, line size = " + QString::number(line.size()));
                break;
            }

            bot b;
            b.posX = line[0].toFloat();
            b.posY = line[1].toFloat();
            b.dirX = line[2].toFloat();
            b.dirY = line[3].toFloat();
            b.radius = line[4].toFloat();
            b.v = line[5].toFloat();
            b.bX = line[6].toFloat();
            b.bY = line[7].toFloat();
            b.laX = line[8].toFloat();
            b.laY = line[9].toFloat();
            b.alive = true;
            (*data[frameNum]).push_back(b);
            bi++;
        }
        data.pop_back();
        frameNum = data.size();
        if( frameNum == 0 )
        {
            log("LogLoader: readed no frame");
            okay = false;
        }
        else
            log("LogLoader: readed " + QString::number(frameNum) + " frames with " + QString::number(botNum) + " bots each.");
    }
    else
    {
        botNum=0;
        frameNum=0;
        log("LogLoader: couldn't open");
    }
    file->close();
    file.reset();
    if(!watcher.files().empty())
        watcher.removePaths(watcher.files());
    watcher.addPath(path);
}

void LogLoader::readObs(QString path)
{
    QFile file(path);//TODO: do this
    bool tokay = file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(tokay)
    {
        walls.clear();
        QTextStream in(&file);

        while(!in.atEnd())
        {
            QString line = in.readLine();
            if(line[0]=='#')//comment
                continue;
            else if(line[0] == '0')//тип препьятсвия
            {
                QStringList sl = line.split(' ');
                if(sl.size()!=5)
                {
                    log("LogLoader: bad map file");
                    break;
                }
                //skipped collision check(may be needed, or not)
                walls.push_back(QLineF(sl[1].toDouble(), sl[2].toDouble(), sl[3].toDouble(), sl[4].toDouble()));
                continue;
            }
            else
            {
                log("LogLoader: bad map file");
                break;
            }
        }
        log("LogLoader: loaded " + QString::number(walls.size()) + " obstacles");
    }
    else
        log("LogLoader: couldn't load obstacles file");
    file.close();
    //skipped process_obstacles(not needed)

    if(!obsWatcher.files().empty())
        obsWatcher.removePaths(obsWatcher.files());
    obsWatcher.addPath(path);
}

bool LogLoader::isObsFileChanged()
{
    if(bObsChanged)
    {
        bObsChanged = false;
        return true;
    }
    return false;
}

void LogLoader::fileChanged(const QString &path)
{
    log("LogLoader: Watcher: " + path + " changed.");
    read(path);
}

void LogLoader::obsFileChanged(const QString &path)
{
    log("LogLoader: Watcher: obstacles file " + path + " changed.");
    readObs(path);
    bObsChanged = true;
}
