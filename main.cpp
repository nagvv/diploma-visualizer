#include "common.h"
#include "findfiles.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    int returnCode = a.exec();

    return returnCode;
}
