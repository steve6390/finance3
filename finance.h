#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextStream>
#include <QTableWidget>
#include <QKeyEvent>
#include <QPixmap>
#include <QCursor>
#include <QVector>

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
    void leftTableMoveRight(int row);
    void midTableMoveLeft(int row);
    void midTableMoveRight(int row);
    void rightTableMoveLeft(int row);

private:
    Ui::finance *ui;
    QPixmap leftCursorPixmap = QPixmap(":/images/cursor_left.png");
    QCursor leftCursor = QCursor(leftCursorPixmap, 0, 21);
    QPixmap rightCursorPixmap = QPixmap(":/images/cursor_right.png");
    QCursor rightCursor = QCursor(rightCursorPixmap, 53, 21);
    QPixmap bothCursorPixmap = QPixmap(":/images/cursor_both.png");
    QCursor bothCursor = QCursor(bothCursorPixmap, 3, 14);

    void moveRow(QTableWidget* fromTable, QTableWidget* toTable, int row);

};

#endif // MAINWINDOW_H
