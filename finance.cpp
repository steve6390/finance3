#include "finance.h"
#include "helpers.h"
#include "ui_finance.h"
#include "financetablewidget.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QStringList>
#include <QtDebug>
#include <QSettings>
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

    // Restore settings
    readSettings();
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
        ignoreColumns.append(settings.value("Description").toString());
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


// clean up unwanted characters from each entry in the QStringList
void sanitize(QStringList& list){
  QMutableStringListIterator i(list);
  while(i.hasNext()){
    QString& s = i.next();
    s.remove('\"');
    s.remove('\n');
  }
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
    sanitize(headerList);

    // We've finished processing the header row, so remove
    // it from lines to prevent the header from becoming
    // a table entry.
    lines.removeFirst();

    descriptionColumn = headerList.indexOf("Description");
    if(descriptionColumn == -1) {
        QMessageBox::critical(this, tr("Error"), tr("Could not find 'Description' column!"));
        return;
    }

    amountColumn = headerList.indexOf("Amount");
    if(amountColumn == -1) {
        QMessageBox::critical(this, tr("Error"), tr("Could not find 'Amount' column!"));
        return;
    }

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

    for(const auto& line : lines) {
        qDebug() << "Processing: " << line << endl;
        QStringList wordList;
        wordList.append(line.split(','));
        sanitize(wordList);

        int row = ui->midTable->rowCount();
        ui->midTable->insertRow(row);

        for(int col = 0, numCols = wordList.length(); col < numCols; col++)
            ui->midTable->setItem(row, col, new QTableWidgetItem(wordList.at(col)));

        // DEBUG
        if(row > 50)
            break;
    }

    // hide ignored columns in all tables
    QMutableStringListIterator i(ignoreColumns);
    while(i.hasNext()){
        int col = headerList.indexOf(i.next());
        if(col == -1)
            continue;
        ui->leftTable->hideColumn(col);
        ui->midTable->hideColumn(col);
        ui->rightTable->hideColumn(col);
    }

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

void finance::movePredeterminedRows(QTableWidget* fromTable, QTableWidget* toTable,
                                    const QStringList& list) {
    QStringListIterator i(list);
    while(i.hasNext()) {
        const QString s = i.next();
        qDebug() << "Checking for pre-determined item: " << s << endl;
        for(int row = 1, end = fromTable->rowCount(); row < end; row++) {
            if(fromTable->item(row, descriptionColumn)->text() == s) {
                qDebug() << "Found on row: " << row << endl;
                moveRow(fromTable, toTable, row);
                // we just reduced the max row count by 1
                end--;
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
