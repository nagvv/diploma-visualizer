#ifndef COMMON_H
#define COMMON_H

#include <QObject>
#include <QTextStream>
#include <QProcess>
#include <QDebug>
#include "visualizer.h"
#include "mainwindow.h"

class Common : public QObject
{
    Q_OBJECT
private:
    QString execFilePath, configFilePath;
    QTextStream out;
    QMap<int, Visualizer*> vises;
    int next_id;
    QProcess *process;
    MainWindow *pFather;

public:
    explicit Common(QObject *parent = nullptr);
    void log(QString s);
    int addVis(QWidget *parent);
    void removeVis(int);
    void clearVises();
    void runExec(QWidget *parent, bool isMPI, int pnum);
    void stopExec();

    void setExecPath(const QString &s);
    void setConfigPath(const QString &s);
    QString getExecPath();
    QString getConfigPath();
signals:
    void pStartedSignal();
    void pStateChangedSignal(QProcess::ProcessState);
    void pFinishedSignal(int exitCode, QProcess::ExitStatus exitStatus);
    void pErrorOccuredSignal(QProcess::ProcessError);

public slots:
    void pStarted();
    void pStateChanged(QProcess::ProcessState);
    void pFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void pErrorOccured(QProcess::ProcessError);

};

extern Common common;
void log(QString s);

#endif // COMMON_H
