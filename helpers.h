#ifndef HELPERS_H
#define HELPERS_H

#include <QStringList>
#include <QVector>
#include <QVectorIterator>
#include <QTableWidget>
#include <QRegExp>

using StringListVec = QVector<QStringList>;
using StringListVecItor = QVectorIterator<QStringList>;

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

/**
 * @brief getDateFromString Given a string mm/dd/yyyy format, return
 * integers from month, day, year.  Month a day may be single digits.
 * Year must be 4 digits.
 * @param str
 * @param day
 * @param month
 * @param year
 */
void getDateFromString(const QString& str, int* day, int* month, int* year);

#endif // HELPERS_H
