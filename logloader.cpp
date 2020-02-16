#include <cstdint>
#include "logloader.h"
#include <QDataStream>
#include "common.h"

LogLoader::LogLoader() : frameNum(0), bObsChanged(false)
{
    connect(&watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(fileChanged(const QString&)), Qt::QueuedConnection);
    connect(&obsWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(obsFileChanged(const QString&)), Qt::QueuedConnection);
}

LogLoader::LogLoader(QString path) : frameNum(0), bObsChanged(false)
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
    okay = file->open(QIODevice::ReadOnly);
    if(okay)
    {
        log("LogLoader: successfully opened");
        QDataStream in(file.get());

        uint32_t lastStep = 0;
        uint32_t step = 0;
        data.clear();
        data.emplace_back(make_shared<vector<bot>>());
        while(!in.atEnd())
        {
            in.readRawData(reinterpret_cast<char *>(&step), sizeof(step));
            if (step != lastStep)
            {
                lastStep = step;
                data.emplace_back(make_shared<vector<bot>>());
            }

            bot b;
            in.readRawData(reinterpret_cast<char *>(&b.posX), sizeof(step));
            in.readRawData(reinterpret_cast<char *>(&b.posY), sizeof(step));
            in.readRawData(reinterpret_cast<char *>(&b.dirX), sizeof(step));
            in.readRawData(reinterpret_cast<char *>(&b.dirY), sizeof(step));
            in.readRawData(reinterpret_cast<char *>(&b.radius), sizeof(step));
            in.readRawData(reinterpret_cast<char *>(&b.v), sizeof(step));
            in.readRawData(reinterpret_cast<char *>(&b.bX), sizeof(step));
            in.readRawData(reinterpret_cast<char *>(&b.bY), sizeof(step));
            in.readRawData(reinterpret_cast<char *>(&b.laX), sizeof(step));
            in.readRawData(reinterpret_cast<char *>(&b.laY), sizeof(step));
            b.alive = true;
            (*data.back()).push_back(b);
        }
        data.pop_back();
        frameNum = data.size();
        if (frameNum == 0)
        {
            log("LogLoader: readed no frame");
            okay = false;
        }
        else
            log("LogLoader: readed " + QString::number(frameNum) + " frames");
    }
    else
    {
        frameNum = 0;
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
    QFile file(path); // TODO: do this
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
