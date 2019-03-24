#include "helpers.h"

// takes and returns the whole row
QList<QTableWidgetItem*> takeRow(QTableWidget* tbl, int row)
{
    QList<QTableWidgetItem*> rowItems;
    for (int col = 0, colCount = tbl->columnCount(); col < colCount; ++col)
        rowItems << tbl->takeItem(row, col);
    tbl->removeRow(row);
    return rowItems;
}

// sets the whole row
void addRow(QTableWidget* tbl, const QList<QTableWidgetItem*>& rowItems)
{
    if( rowItems.empty())
        return;

    int row = tbl->rowCount();
    tbl->insertRow(row);

    for (int col = 0, colCount = tbl->columnCount(); col < colCount; ++col)
        tbl->setItem(row, col, rowItems.at(col));
}
