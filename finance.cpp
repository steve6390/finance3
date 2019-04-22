#include "finance.h"
#include "ui_finance.h"
#include "financetablewidget.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QStringList>
#include <QtDebug>
#include <QSettings>
#include <QVector>
#include <math.h>

finance::finance(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::finance)
{
    ui->setupUi(this);

    connect(ui->leftTable, &FinanceTableWidget::moveRight,
            this, &finance::leftTableMoveRight);
    connect(ui->midTable, &FinanceTableWidget::moveLeft,
            this, &finance::midTableMoveLeft);
    connect(ui->midTable, &FinanceTableWidget::moveRight,
            this, &finance::midTableMoveRight);
    connect(ui->rightTable, &FinanceTableWidget::moveLeft,
            this, &finance::rightTableMoveLeft);
    connect(ui->dateEdit, &QDateEdit::dateChanged,
            this, &finance::dateUpdate);

    // Restore settings
    readSettings();
}

void finance::dateUpdate(const QDate &date) {
    qDebug() << "Date update! " << date << endl;
    resetTables();
}

void finance::closeEvent(QCloseEvent* event){
    // Save our current configuration
    qDebug() << "Closing up!" << endl;
    writeSettings();
    QMainWindow::closeEvent(event);
}

void finance::writeSettings() {
    QSettings settings("Personal", "Finance");

    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();

    settings.beginWriteArray("IgnoreColumns");
    for(int i = 0, end = ignoreColumns.count(); i < end; i++) {
        settings.setArrayIndex(i);
        settings.setValue("name",ignoreColumns.at(i));
    }
    settings.endArray();

    settings.beginWriteArray("Right");
    for(int i = 0, end = jointList.count(); i < end; i++) {
        settings.setArrayIndex(i);
        settings.setValue("Description",jointList.at(i));
    }
    settings.endArray();

    settings.beginWriteArray("Left");
    for(int i = 0, end = mineList.count(); i < end; i++) {
        settings.setArrayIndex(i);
        settings.setValue("Description",mineList.at(i));
    }
    settings.endArray();

}

void finance::readSettings()
{
    QSettings settings("Personal", "Finance");

    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(1024, 768)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();

    int end = settings.beginReadArray("IgnoreColumns");
    for(int i = 0; i < end; i++) {
        settings.setArrayIndex(i);
        ignoreColumns.append(settings.value("name").toString());
    }
    settings.endArray();

    end = settings.beginReadArray("Right");
    for(int i = 0; i < end; i++) {
        settings.setArrayIndex(i);
        jointList.append(settings.value("Description").toString());
    }
    settings.endArray();

    end = settings.beginReadArray("Left");
    for(int i = 0; i < end; i++) {
        settings.setArrayIndex(i);
        mineList.append(settings.value("Description").toString());
    }
    settings.endArray();

}

void finance::moveRow(QTableWidget* fromTable, QTableWidget* toTable, int row) {
    QList<QTableWidgetItem*> rowList;
    rowList = takeRow(fromTable, row);
    addRow(toTable, rowList);
}


finance::~finance() {
    delete ui;
}



void finance::on_actionOpen_triggered()
{
    qDebug() << "I'm in the action!";
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
            tr("CSV Files (*.csv) ;; All files (*.*)"));

    qDebug() << "Selected file name is " << fileName << endl;

    if(fileName.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
        return;
    }

    if(file.atEnd()) {
        QMessageBox::critical(this, tr("Error"), tr("File is empty!"));
        file.close();
        return;
    }


    // read all lines of the file in a QStringList
    QStringList lines;
    while(!file.atEnd())
        lines.append(file.readLine());
    file.close();

    // Get the list of comma separated header row titles
    QStringList headerList;
    headerList.append(lines.at(0).split(','));
    sanitize(&headerList);

    // We've finished processing the header row, so remove
    // it from lines to prevent the header from becoming
    // a table entry.
    lines.removeFirst();

    // As we get column info, also calculate the largest required column.
    // We don't care about per row length variations larger than this column.
    descriptionColumn = headerList.indexOf("Description");
    if(descriptionColumn == -1) {
        QMessageBox::critical(this, tr("Error"), tr("Could not find 'Description' column!"));
        return;
    }
    minRequiredCol = std::max(minRequiredCol, descriptionColumn);

    amountColumn = headerList.indexOf("Amount");
    if(amountColumn == -1) {
        QMessageBox::critical(this, tr("Error"), tr("Could not find 'Amount' column!"));
        return;
    }
    minRequiredCol = std::max(minRequiredCol, amountColumn);

    dateColumn = headerList.indexOf("Date");
    if(amountColumn == -1) {
        QMessageBox::critical(this, tr("Error"), tr("Could not find 'Date' column!"));
        return;
    }
    minRequiredCol = std::max(minRequiredCol, dateColumn);

    // make sure our table has enough columns
    int numCols = ui->midTable->columnCount();
    for(int end = headerList.length(); numCols < end; numCols++) {
        ui->leftTable->insertColumn(numCols);
        ui->midTable->insertColumn(numCols);
        ui->rightTable->insertColumn(numCols);
    }

    ui->leftTable->setHorizontalHeaderLabels(headerList);
    ui->midTable->setHorizontalHeaderLabels(headerList);
    ui->rightTable->setHorizontalHeaderLabels(headerList);

    // Convert raw csv lines into vector of string lists
    buildRowsVec(lines, &fileRowsVec);

    // Remove runt or oversize rows from the rows vector
    normalizeRowsVec(&fileRowsVec, minRequiredCol);

    // Hide ignored columns per user settings
    hideIgnoredColumns(headerList);

    // Setup tables for the selected date range.
    // Currently, the range is all days in the specified month.
    resetTables();
}

void finance::hideIgnoredColumns(const QStringList& hdrList) {
    // hide ignored columns in all tables
    QMutableStringListIterator i(ignoreColumns);
    while(i.hasNext()){
        int col = hdrList.indexOf(i.next());
        if(col == -1)
            continue;
        ui->leftTable->hideColumn(col);
        ui->midTable->hideColumn(col);
        ui->rightTable->hideColumn(col);
    }
}

void finance::initMonthVec() {
    // clear existing entrys from the month vec
    monthRowsVec.clear();
    StringListVecItor i(fileRowsVec);
    const QDate& date = ui->dateEdit->date();
    while(i.hasNext()) {
        const QStringList& qsl = i.next();
        // Date format string is "mm/dd/yyyy" where month and day
        // can be single digits.

        int day = 0;
        int month = 0;
        int year = 0;

        QString strDate = qsl.at(dateColumn);
        getDateFromString(strDate, &day, &month, &year);
        // if the date is the right month and year, then copy
        // the StringList to the month vector.
        if(year == date.year() and month == date.month())
            monthRowsVec.append(qsl);
    }
}

void finance::resetTables() {

    // Create the StringListVec for the specified month
    initMonthVec();

    // Populate the middle table with our vector of string lists
    setRows(ui->midTable, monthRowsVec);

    movePredeterminedRows(ui->midTable, ui->rightTable, jointList);
    movePredeterminedRows(ui->midTable, ui->leftTable, mineList);
    calculateTotals();
}

void finance::calcTotal(const QTableWidget& tbl, QLabel* totalLabel) {
    double total = 0;
    for(int row = 0, end = tbl.rowCount(); row < end; row++) {
        QString amtString = tbl.item(row, amountColumn)->text();
        double amtVal = amtString.toDouble();
        total += amtVal;
    }

    // Storing dollars and cents in a double
    // has surprising complexities.  In our case
    // we have enough precision with a double,
    // so just print with 2 decimals of precision
    // for the cents.

    QString txt = "Total $" + QString::number(total, 'f', 2);
    totalLabel->setText(txt);
}

void finance::calculateTotals() {
    calcTotal(*ui->leftTable, ui->leftTotal);
    calcTotal(*ui->midTable, ui->midTotal);
    calcTotal(*ui->rightTable, ui->rightTotal);
}

void finance::movePredeterminedRows(QTableWidget* fromTable,
                                    QTableWidget* toTable,
                                    const QStringList& list) {
    QStringListIterator i(list);
    while(i.hasNext()) {
        const QString& s = i.next();
        qDebug() << "Checking for pre-determined item: " << s << endl;
        for(int row = 0, rowEnd = fromTable->rowCount(); row < rowEnd; row++) {
            const QTableWidgetItem* wi = fromTable->item(row, descriptionColumn);
            if(wi == nullptr) {
                qDebug() << "Null widget item on row " << row
                         << ", column " << descriptionColumn;
                for(int c = 0, colEnd = fromTable->columnCount(); c < colEnd; c++) {
                    const QTableWidgetItem* xwi = fromTable->item(row, c);
                    qDebug() << "Col " << c << ": ";
                    if(xwi != nullptr) {
                        qDebug() << "    " << xwi->text();
                    } else {
                        qDebug() << "    null!";
                    }
                }
                QMessageBox::critical(this, tr("Error"),
                                      tr("table has null description!"));
                return;
            }
            if(wi->text() == s) {
                qDebug() << "    Found on row: " << row;
                moveRow(fromTable, toTable, row);
                // we just reduced the max row count by 1
                rowEnd--;
            }
        }
    }
}

void finance::leftTableMoveRight(int row) {
    moveRow(ui->leftTable, ui->midTable, row);
    calculateTotals();
}

void finance::midTableMoveLeft(int row) {
    moveRow(ui->midTable, ui->leftTable, row);
    calculateTotals();
}

void finance::midTableMoveRight(int row) {
    moveRow(ui->midTable, ui->rightTable, row);
    calculateTotals();
}

void finance::rightTableMoveLeft(int row) {
    moveRow(ui->rightTable, ui->midTable, row);
    calculateTotals();
}

void finance::setRows(QTableWidget* tbl, const StringListVec& lv) {
    // remove existing rows
    tbl->setRowCount(0);

    // create new rows
    const int numRows = lv.length();

    tbl->setRowCount(numRows);

    // Walk the StringListVec populating rows and cols
    // Reject any runt rows
    for(int row = 0; row < numRows; row++) {
        const QStringList& qsl = lv.at(row);

        // DEBUG TOP
        if(row == 272) {
            qDebug() << "DEBUG row 272: " << qsl << endl;
        }
        // DEBUG BOTTOM

        // Rows have varying length
        const int colMax = qsl.length();
        for(int col = 0; col < colMax; col++) {
            QTableWidgetItem* twi = new QTableWidgetItem(qsl.at(col));
            if(twi == nullptr) {
                QMessageBox::critical(this, tr("Error"), tr("Out of memory creating QTableWidgetItem"));
                exit(-1);
            }
            tbl->setItem(row, col, twi);
        }
    }
}

void finance::on_saveLeftButton_clicked() {
    qDebug() << "I'm in on_saveLeftButton_Clicked()!";
    QFileDialog saveAsDlg;
    saveAsDlg.selectFile("CoolDefault.csv");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Save File"), QString(),
            tr("CSV Files (*.csv) ;; All files (*.*)"));

    qDebug() << "Selected file name is " << fileName << endl;
    if(printTableAsCSV(ui->leftTable, fileName)) {
        QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
        return;
    }
}
