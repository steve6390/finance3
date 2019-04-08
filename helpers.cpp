#include "helpers.h"
#include <QDebug>
#include <QRegExp>

// Returns a whole row and removes that row from the table.
QList<QTableWidgetItem*> takeRow(QTableWidget* tbl, int row)
{
    QList<QTableWidgetItem*> rowItems;
    for (int col = 0, colCount = tbl->columnCount(); col < colCount; ++col)
        rowItems << tbl->takeItem(row, col);
    tbl->removeRow(row);
    return rowItems;
}

// Adds a row from a list of widget items.
void addRow(QTableWidget* tbl, const QList<QTableWidgetItem*>& rowItems)
{
    if( rowItems.empty())
        return;

    int row = tbl->rowCount();
    tbl->insertRow(row);

    for (int col = 0, colCount = tbl->columnCount(); col < colCount; ++col)
        tbl->setItem(row, col, rowItems.at(col));
}

void buildRowsVec(const QStringList& lines, StringListVec* rv) {
    for(const auto& line : lines) {
        // qDebug() << "Processing: " << line << endl;
        QStringList wordList;
        wordList.append(line.split(','));
        sanitize(&wordList);
        rv->append(wordList);
    }
}

void normalizeRowsVec(StringListVec* rv, int minLength) {
    int numRows = rv->length();
    for(int r = 0; r < numRows; r++) {
        if(rv->at(r).length() < minLength) {
            qDebug() << "Warning: ignoring short row " << r << ": "
                     << rv->at(r) << endl;
            rv->remove(r);
            --numRows; // we decreased total rows
            --r; // and we effectly changed current index
        }
    }
}

void sanitize(QStringList* list) {
  QMutableStringListIterator i(*list);
  while(i.hasNext()) {
    QString& s = i.next();
    s.remove('\"');
    s.remove('\n');
  }
}

void getDateFromString(const QString& str, int* day, int* month, int* year) {
    static const QRegExp rx("(\\d+)/(\\d+)/(\\d\\d\\d\\d)");
    int pos = rx.indexIn(str);
    if(pos == -1) {
        qDebug() << "Error, coudn't extract date from: " << str;
        exit(-1);
    }
    // The first captured string is the entire substring

    QStringList ql = rx.capturedTexts();
    if(ql.length() != 4) {
        qDebug() << "Error, bad date extract date from: " << str;
        exit(-1);
    }

    *month = ql.at(1).toInt();
    *day = ql.at(2).toInt();
    *year = ql.at(3).toInt();
}
