#ifndef HELPERS_H
#define HELPERS_H

#include <QStringList>
#include <QVector>
#include <QTableWidget>

using StringListVec = QVector<QStringList>;

/**
 * @brief takeRow Removes the specified row from a table widget.
 * @param tbl
 * @param row
 * @return The removed row as a list of widget items.
 */
QList<QTableWidgetItem*> takeRow(QTableWidget* tbl, int row);

/**
 * @brief addRow Adds the specified row to a table widget.
 * @param tbl
 * @param rowItems
 */
void addRow(QTableWidget* tbl, const QList<QTableWidgetItem*>& rowItems);

/**
 * @brief addRows Sets all rows in the table widget.  Existing
 * rows are deleted.
 * @param tbl
 * @param lv
 */
void setRows(QTableWidget* tbl, const StringListVec& lv);

/**
 * @brief buildRowsVec Given lines from a csv file,  populate a QVector of
 * QStringLists in which each vector element is a row of the file.
 * @param lines QStringList of csv file lines.
 * @param rv Pointer to QVector of QStringLists
 */
void buildRowsVec(const QStringList& lines, StringListVec* rv);

/**
 * @brief sanitize Removes \n and \" characters from each element
 * of a string list.
 * @param list to clean.
 */
void sanitize(QStringList* list);

/**
 * @brief normalizeRowsVec Removes any list in the vector with a length
 * not equal to the expected length.
 * @param rv
 * @param minLength
 */
void normalizeRowsVec(StringListVec* rv, int minLength);

#endif // HELPERS_H
