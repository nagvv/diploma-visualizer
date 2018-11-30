#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <QWidget>
#include <QCloseEvent>
#include <QTimer>
#include <QDir>
#include "logloader.h"

namespace Ui {
class Visualizer;
}

class Visualizer : public QWidget
{
    Q_OBJECT
private:
    Ui::Visualizer *ui;
    int id;
    QTimer *updater;
    QDir dir;
    LogLoader ll;
    size_t frame;
    int spcu;
    int spto;
    bool pause;

    //moving
    bool mvLeft = false;
    bool mvRight = false;
    bool mvUp = false;
    bool mvDown = false;
    bool mvZoomIn = false;
    bool mvZoomOut = false;

public:
    explicit Visualizer(int id, QWidget *parent = nullptr);
    ~Visualizer();
    void closeEvent(QCloseEvent *event);

public slots:
    void vUpdate();

private slots:
    void keyPressEvent(QKeyEvent* key);
    void keyReleaseEvent(QKeyEvent *key);

    void on_exitButton_clicked();

    void on_centerButton_clicked();

    void on_leftButton_clicked();

    void on_rightButton_clicked();

    void on_upButton_clicked();

    void on_downButton_clicked();

    void on_zoomInButton_clicked();

    void on_zoomOutButton_clicked();

    void on_resetZoomButton_clicked();

    void on_resetButton_clicked();


    void on_playPauseButton_clicked();

    void on_incSpeedButton_clicked();

    void on_decSpeedButton_clicked();

    void on_resetSpeedButton_clicked();

    void on_restartButton_clicked();
    void on_chooseBox_currentIndexChanged(const QString &file);

    void on_chooseButton_clicked();
    void on_shCenterCheckBox_stateChanged(int set);
    void on_shLookAtCheckBox_stateChanged(int set);
    void on_shBestPosCheckBox_stateChanged(int set);
    void on_obsButton_clicked();
    void on_shTracesCheckBox_stateChanged(int set);
    void on_colorBotsChBox_stateChanged(int set);
};

#endif // VISUALIZER_H
