#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextStream>
#include <QTableWidget>

namespace Ui {
class finance;
}

class finance : public QMainWindow
{
    Q_OBJECT

public:
    explicit finance(QWidget *parent = 0);
    ~finance();

private slots:
    void on_actionOpen_triggered();

private:
    Ui::finance *ui;

};

#endif // MAINWINDOW_H
