#ifndef HELPERS_H
#define HELPERS_H

#include <QList>
#include <QTableWidget>

// Take a whole row
QList<QTableWidgetItem*> takeRow(QTableWidget* tbl, int row);

// adds a whole row
void addRow(QTableWidget* tbl, const QList<QTableWidgetItem*>& rowItems);

#endif // HELPERS_H
