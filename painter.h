#ifndef PAINTER_H
#define PAINTER_H

#include <QWidget>
#include "data.h"

class Painter : public QWidget
{
    Q_OBJECT
public:
    explicit Painter(QWidget *parent = nullptr);
    void paint(struct data td);

private:
    struct data d;

protected:
    void paintEvent(QPaintEvent *event);

signals:

public slots:
};

#endif // PAINTER_H
