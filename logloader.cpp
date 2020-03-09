#include <cstdint>
#include "logloader.h"
#include <QDataStream>
#include <QFileInfo>
#include <QDir>
#include "common.h"

namespace {
    QString getGridPath(const QString &path)
    {
        QFileInfo info(path);
        QString fname = info.fileName();
        if (fname.left(3) != "log")
            return QString();
        return info.path() + QDir::separator() + "grid" + fname.mid(3);
    }
}

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
        {
            log("LogLoader: readed " + QString::number(frameNum) + " frames");
            readGrid(getGridPath(path));
        }
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

void LogLoader::readGrid(QString path)
{
    grid_file = std::unique_ptr<QFile>(new QFile(path));
    log("LogLoader: opening " + path);
    okay = grid_file->open(QIODevice::ReadOnly);
    if(okay)
    {
        log("LogLoader: successfully opened");
        QDataStream in(grid_file.get());
        grid_data.clear();
        while(!in.atEnd())
        {
            grid _grid;
            in.readRawData(reinterpret_cast<char *>(&_grid.grid_size), sizeof(_grid.grid_size));
            in.readRawData(reinterpret_cast<char *>(&_grid.area_size), sizeof(_grid.area_size));
            _grid.cells.resize(_grid.grid_size * _grid.grid_size);
            in.readRawData(reinterpret_cast<char *>(_grid.cells.data()), sizeof(float) * _grid.cells.size());
            grid_data.push_back(std::move(_grid));
        }
        grid_data.resize(data.size());
    }
    else
        log("LogLoader: couldn't open " + path);
    grid_file->close();
    grid_file.reset();
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
