#include "logloader.h"
#include <QTextStream>
#include "common.h"

LogLoader::LogLoader() : botNum(0), frameNum(0)
{
    connect(&watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(fileChanged(const QString&)));

    QFile file("obs1.txt");
    okay = file.open(QIODevice::ReadOnly | QIODevice::Text);
    if(okay)
    {
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
                walls.push_back(QLine(sl[1].toFloat(), sl[2].toFloat(), sl[3].toFloat(), sl[4].toFloat()));
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
}

LogLoader::LogLoader(QString path)
{
    connect(&watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(fileChanged(const QString&)));
    read(path);
}

LogLoader::~LogLoader()
{
    disconnect(&watcher, SIGNAL(fileChanged(const QString&)), this, SLOT(fileChanged(const QString&)));
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
        data.emplace_back();
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
                    data.emplace_back();
                    continue;
                }
                else
                {
                    okay = false;
                    log("LogLoader: bad file _0");
                    break;
                }
            }
            else if(line.size() != 10)
            {
                okay = false;
                log("LogLoader: bad file _1, line size = " + QString::number(line.size()));
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
            data[frameNum].push_back(b);
            bi++;
        }
        data.pop_back();
        frameNum = data.size();
        log("LogLoader: readed " + QString::number(frameNum) + " frames with " + QString::number(botNum) + " bots each.");
    }
    else
    {
        botNum=0;
        frameNum=0;
        log("LogLoader: couldn't open");
    }
    file.reset();
    if(!watcher.files().empty())
     watcher.removePaths(watcher.files());
    watcher.addPath(path);
}

void LogLoader::fileChanged(const QString &path)
{
    log("LogLoader: Watcher: " + path + " changed.");
    read(path);
}
