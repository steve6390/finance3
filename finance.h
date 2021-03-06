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
#include "tablecontextmenu.h"

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

    void on_saveLeftButton_clicked();

    void on_saveMidButton_clicked();

    void on_saveRightButton_clicked();

    void on_leftTable_customContextMenuRequested(const QPoint &pos);
    void on_rightTable_customContextMenuRequested(const QPoint &pos);

    void on_addNewButton_clicked();

private:
    Ui::finance *ui;
    QPixmap leftCursorPixmap = QPixmap(":/images/cursor_left.png");
    QCursor leftCursor = QCursor(leftCursorPixmap, 0, 21);
    QPixmap rightCursorPixmap = QPixmap(":/images/cursor_right.png");
    QCursor rightCursor = QCursor(rightCursorPixmap, 53, 21);
    QPixmap bothCursorPixmap = QPixmap(":/images/cursor_both.png");
    QCursor bothCursor = QCursor(bothCursorPixmap, 3, 14);

    void customContextMenuRequested(const QPoint &pos,
                                    const QTableWidget* tbl,
                                    QStringList* permList);

    void moveRow(QTableWidget* fromTable, QTableWidget* toTable, int row);
    void movePredeterminedRows(QTableWidget* fromTable, QTableWidget* toTable,
                               const QStringList& list);

    void setRows(QTableWidget* tbl, const StringListVec& lv);

    void calcTotal(const QTableWidget& tbl, QLabel* total);

    void saveTable(const QString& tblName, const QString& totalText,
                   const QTableWidget* tbl);

    void calculateTotals();
    void movePredeterminedRows();

    void resetTables();
    void hideIgnoredColumns(const QStringList& hdrList);
    void initColumns(const QStringList& hdrList);

    /**
     * getDirPart Returns the directory part of the path as a QString.
     */
    QString getDirPart(const QString& path);

    void initMonthVec();

    void writeSettings();
    void readSettings();

    QStringList ignoreColumns;
    QStringList leftPermList;
    QStringList rightPermList;

    QString lastLoadDir;
    QString lastSaveDir;

    TableContextMenu tblContextMenu;

    // Interesting column numbers set in on_actionOpen_triggered
    int descriptionColumn = -1;
    int amountColumn = -1;
    int dateColumn = -1;
    int transactionTypeColumn = -1;

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
