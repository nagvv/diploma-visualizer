#ifndef FINDFILES_H
#define FINDFILES_H

#include <QDialog>
#include <QFileDialog>

namespace Ui {
class findFiles;
}

class findFiles : public QDialog
{
    Q_OBJECT

public:
    explicit findFiles(QWidget *parent = nullptr);
    ~findFiles();

private slots:

    void on_execBrowse_clicked();

    void on_configBrowse_clicked();

    void on_buttonBox_accepted();

private:
    Ui::findFiles *ui;
};
#endif // FINDFILES_H
