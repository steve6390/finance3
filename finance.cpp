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

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
            return;
        }


        QString sign = "";
        while (!file.atEnd()) {
            QString line = file.readLine();
            line.remove("\"");
            QStringList wordList;
            wordList.append(line.split(','));
            qDebug() << "Processing: " << line << endl;
            if(wordList.length() < 5) {
                qDebug() << "Ignoring line!\n";
                continue;
            }
            int row = ui->midTable->rowCount();
            ui->midTable->insertRow(row);

            ui->midTable->setItem(row, 0, new QTableWidgetItem(wordList.at(0)));
            ui->midTable->setItem(row, 1, new QTableWidgetItem(wordList.at(1)));
            if(wordList.at(4) == "credit")
                sign = "-";
            else
                sign = "";
            ui->midTable->setItem(row, 2, new QTableWidgetItem(sign + wordList.at(3)));

            // DEBUG
            if(row > 30)
                break;
        }

        file.close();
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
