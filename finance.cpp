#include "finance.h"
#include "helpers.h"
#include "ui_finance.h"
#include "financetablewidget.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QStringList>
#include <QtDebug>

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

    // Column names to ignore
    // Todo: get these from a user config file
    ignoreColumns.append("Category");
    ignoreColumns.append("Labels");
    ignoreColumns.append("Account Name");
    ignoreColumns.append("Original Description");
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
    i.next().remove('\"');
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
        if(row > 30)
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
}

void finance::leftTableMoveRight(int row) {
    moveRow(ui->leftTable, ui->midTable, row);
}
void finance::midTableMoveLeft(int row) {
    moveRow(ui->midTable, ui->leftTable, row);
}
void finance::midTableMoveRight(int row) {
    moveRow(ui->midTable, ui->rightTable, row);
}
void finance::rightTableMoveLeft(int row) {
    moveRow(ui->rightTable, ui->midTable, row);
}
