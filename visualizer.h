#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <QWidget>
#include <QCloseEvent>

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

private:
    Ui::Visualizer *ui;
    int id;
};

#endif // VISUALIZER_H
