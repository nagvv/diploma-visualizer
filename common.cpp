#include "common.h"

Common::Common(QObject *parent) : QObject(parent)
{
    pRunning = false;
    execFilePath = "";
    configFilePath= "";
    QFile stdoutFile;
    stdoutFile.open(stdout, QIODevice::WriteOnly);
    out.setDevice(&stdoutFile);
    next_id=0;
}

void Common::log(QString s)
{
    qDebug() << "LOG: " << s;
}

int Common::addVis(QWidget *parent)
{
    int id = next_id++;
    Visualizer *vis = new Visualizer(id, parent);
    vis->show();
    vises.insert(id, vis);
    log(QString("added visualizer ") + QString::number(id));
    return id;
}

void Common::removeVis(int id)
{
    Visualizer *vis = vises.take(id);
    vis->close();
    delete vis;
    log(QString("removed visualizer ") + QString::number(id));
}

void Common::runExec(QWidget *parent, bool isMPI, int pnum)
{
    process = new QProcess(parent);
    QString file = execFilePath;
    QStringList arguments(configFilePath);
    connect(process, SIGNAL(started()), this, SLOT(pStarted()));
    connect(process, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(pStateChanged(QProcess::ProcessState)));
    connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(pFinished(int, QProcess::ExitStatus)));
    connect(process, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(pErrorOccured(QProcess::ProcessError)));
    pFather = dynamic_cast<MainWindow*>(parent);
    connect(this, SIGNAL(pStartedSignal()), parent, SLOT(pStarted()));
    connect(this, SIGNAL(pStateChangedSignal(QProcess::ProcessState)), parent, SLOT(pStateChanged(QProcess::ProcessState)));
    connect(this, SIGNAL(pFinishedSignal(int, QProcess::ExitStatus)), parent, SLOT(pFinished(int, QProcess::ExitStatus)));
    connect(this, SIGNAL(pErrorOccuredSignal(QProcess::ProcessError)), parent, SLOT(pErrorOccured(QProcess::ProcessError)));
    if(isMPI)
    {
        QString rs = QString("mpiexec -n %1 ").arg(pnum) + "\"" + file + "\"";
        log(QString("common: running ") + rs);
        process->start(rs, arguments);

    }
    else
    {
        log(QString("common: running ") + file);
        process->start("\"" + file + "\"");
    }
}

void Common::stopExec()
{
    if(process)
    {
        process->terminate();
        process->waitForFinished(3000);//wait 3 second
        process->kill();
        process->waitForFinished(3000);//wait 3 second
        process->close();
        log("process: terminating...");
    }
}

void Common::setExecPath(const QString &s)
{
    execFilePath = s;
}

void Common::setConfigPath(const QString &s)
{
    configFilePath = s;
}
QString Common::getExecPath()
{
    return execFilePath;
}
QString Common::getConfigPath()
{
    return configFilePath;
}

void Common::clearVises()
{
    vises.clear();
}

void Common::pStarted()
{
    pRunning = true;;
    log("process: started");
    emit pStartedSignal();
}

void Common::pStateChanged(QProcess::ProcessState pState)
{
    log(QString("process: state changed to: %1").arg(pState));
    emit pStateChangedSignal(pState);
}

void Common::pFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    pRunning = false;
    pFather = nullptr;
    log(QString("process: finished with exitCode: %1, and exitStatus: %2").arg(exitCode).arg(exitStatus));
    emit pFinishedSignal(exitCode, exitStatus);
}

void Common::pErrorOccured(QProcess::ProcessError error)
{
    log(QString("process: errorOccured: %1").arg(error));
    emit pErrorOccuredSignal(error);
}


Common common;
void log(QString s)
{
    common.log(s);
}
