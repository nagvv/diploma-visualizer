#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void update();
    void closeEvent(QCloseEvent *event);

private slots:

    void on_chooseBtn_clicked();

    void on_visBtn_clicked();

    void on_isMPI_stateChanged(int arg1);

    void on_startButton_clicked();

    void pStarted();
    void pStateChanged(QProcess::ProcessState);
    void pFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void pErrorOccured(QProcess::ProcessError);

    void on_stopButton_clicked();

    void on_editButton_clicked();

private:
    Ui::MainWindow *ui;
    bool execExist, configExist;

};

#endif // MAINWINDOW_H
