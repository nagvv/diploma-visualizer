#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <QWidget>
#include <QCloseEvent>
#include <QTimer>
#include "data.h"

namespace Ui {
class Visualizer;
}

class Visualizer : public QWidget
{
    Q_OBJECT

public:
    explicit Visualizer(int id, QWidget *parent = nullptr);
    ~Visualizer();
    void closeEvent(QCloseEvent *event);

public slots:
    void vUpdate();

private:
    Ui::Visualizer *ui;
    int id;
    QTimer *repainter;

    struct data d;
};

#endif // VISUALIZER_H
