#include "helpers.h"
#include <QDebug>
#include <QRegExp>
#include <QFile>

// Returns a whole row and removes that row from the table.
QList<QTableWidgetItem*> takeRow(QTableWidget* tbl, int row)
{
    QList<QTableWidgetItem*> rowItems;
    for (int col = 0, colEnd = tbl->columnCount(); col < colEnd; ++col)
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

    for (int col = 0, colEnd = tbl->columnCount(); col < colEnd; ++col)
        tbl->setItem(row, col, rowItems.at(col));
}

void buildRowsVec(const QStringList& lines, StringListVec* rv) {
    for(const auto& line : lines) {
        // qDebug() << "Processing: " << line << endl;
        QStringList wordList;
        wordList.append(csvSplit(line));
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

int printTableAsCSV(const QTableWidget* tbl, const QString& fname,
                    const QString& header, const QString& trailer ) {
    QFile csvFile(fname);
    if(!csvFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return -1;

    QTextStream out(&csvFile);

    QVector<int> max_width_vec(tbl->columnCount());

    // First, find the maximum length text item in each column.
    for (int c = 0, colEnd = tbl->columnCount(); c < colEnd; c++) {
        if(tbl->isColumnHidden(c))
            continue;
        int len_max = 0; // initialize with zero length
        for (int r = 0, rowEnd = tbl->rowCount(); r < rowEnd; r++) {
            const QString temp = tbl->item(r,c)->text();
            len_max = std::max(temp.length(), len_max);
        }
        max_width_vec[c] = len_max;
    }

    out << header;
    for (int r = 0, rowEnd = tbl->rowCount(); r < rowEnd; r++) {
        // Print each table cell in the row with , separation
        for (int c = 0, colEnd = tbl->columnCount(); c < colEnd; c++) {
            if(tbl->isColumnHidden(c))
                continue;
            const QString temp = tbl->item(r,c)->text() + ", ";
            // +2 for the comma and space
            const QString temp2 = temp.rightJustified(max_width_vec[c] + 2);
            out << temp2;
        }
        out << "\n"; // done with row
    }
    out << trailer;
    csvFile.close();
    return 0;
}

QString getMonthName(const QDate& date) {
    static const char* months[12] = { "January", "February", "March",
                                      "April", "May", "June", "July",
                                      "August", "September", "October",
                                      "November", "December"};
    return months[date.month() -1];
}

bool cellMatch(const QTableWidget* tbl, int row, int col, const QString& str) {
    const QTableWidgetItem* wi = tbl->item(row, col);
    if(wi == nullptr)
        return false;
    return wi->text() == str;
}

bool cellMatch(const QTableWidget* tbl, int row, int col,
               const QStringList& strList) {
    const QTableWidgetItem* wi = tbl->item(row, col);
    if(wi == nullptr)
        return false;
    QString wistr = wi->text();
    return strList.contains(wistr);
}

QStringList csvSplit(const QString &str) {
    QStringList result;
    enum State {
        QUOTED,
        COMMA,
        BODY
    };

    State state = BODY;
    const int end = str.length();
    int i = 0;
    QString s;
    while(i < end) {
        const QChar c = str[i];
        switch(state) {

        // Record all until a quote or comma
        case BODY:
            if(c == ',') {
                state = COMMA;
            } else {
                if(c == '\"') {
                    // We found a quoted string
                    state = QUOTED;
                }
                s.append(c);
            }
            ++i; // done with this char
            break;

        case COMMA:
            // Add the current string to the list and start
            // a new string.  The current string may be empty.
            result.push_back(s.trimmed());
            s.clear();
            state = BODY;
            break;

        case QUOTED:
            if(c == '\"') {
                state = BODY;
            }
            s.append(c);
            ++i; // done with this char
            break;
        }
    }
    return result;
}

void setRowBackground(QTableWidget* tbl, const int row, QColor color) {
    for (int col = 0, colEnd = tbl->columnCount(); col < colEnd; ++col) {
        QTableWidgetItem* item = tbl->item(row, col);
        item->setBackgroundColor(color);
    }
}

