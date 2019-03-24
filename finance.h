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
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    void noneTableClickEvent(int row, int column);

private:
    Ui::finance *ui;
    QPixmap leftCursorPixmap = QPixmap(":/images/cursor_left.png");
    QCursor leftCursor = QCursor(leftCursorPixmap, 0, 21);
    QPixmap rightCursorPixmap = QPixmap(":/images/cursor_right.png");
    QCursor rightCursor = QCursor(rightCursorPixmap, 53, 21);

    enum cursorOverrideType {
        none,
        left,
        right
    };

    QVector<cursorOverrideType> override_stack;
};

#endif // MAINWINDOW_H
