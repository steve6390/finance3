#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextStream>
#include <QTableWidget>
#include <QKeyEvent>
#include <QPixmap>
#include <QCursor>
#include <QVector>
#include <QStringList>
#include <QLabel>
#include <QDate>
#include "helpers.h"

namespace Ui {
class finance;
}

class finance : public QMainWindow
{
    Q_OBJECT

public:
    explicit finance(QWidget *parent = 0);
    ~finance();
    void closeEvent(QCloseEvent* event);

private slots:
    void on_actionOpen_triggered();
    void leftTableMoveRight(int row);
    void midTableMoveLeft(int row);
    void midTableMoveRight(int row);
    void rightTableMoveLeft(int row);
    void dateUpdate(const QDate& date);

private:
    Ui::finance *ui;
    QPixmap leftCursorPixmap = QPixmap(":/images/cursor_left.png");
    QCursor leftCursor = QCursor(leftCursorPixmap, 0, 21);
    QPixmap rightCursorPixmap = QPixmap(":/images/cursor_right.png");
    QCursor rightCursor = QCursor(rightCursorPixmap, 53, 21);
    QPixmap bothCursorPixmap = QPixmap(":/images/cursor_both.png");
    QCursor bothCursor = QCursor(bothCursorPixmap, 3, 14);

    void moveRow(QTableWidget* fromTable, QTableWidget* toTable, int row);
    void movePredeterminedRows(QTableWidget* fromTable, QTableWidget* toTable,
                               const QStringList& list);

    void setRows(QTableWidget* tbl, const StringListVec& lv);

    void calcTotal(const QTableWidget& tbl, QLabel* total);
    void calculateTotals();
    void movePredeterminedRows();

    void resetTables();

    void initMonthVec();


    void writeSettings();
    void readSettings();

    QStringList ignoreColumns;
    QStringList jointList;
    QStringList mineList;

    // Interesting column numbers set in on_actionOpen_triggered
    int descriptionColumn = -1;
    int amountColumn = -1;
    int dateColumn = -1;
    // Track the minimum column number we require
    int minRequiredCol = -1;

    // Vector of lists for the entire csv file.
    // Setup in on_actionOpen_triggered.
    // Each entry in the vector is a list of values for that row.
    StringListVec fileRowsVec;

    // Vector of lists for the current month.
    // Setup in on_actionOpen_triggered.
    // Each entry in the vector is a list of values for that row.
    StringListVec monthRowsVec;
};

#endif // MAINWINDOW_H
