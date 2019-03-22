#include "finance.h"
#include "ui_finance.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QStringList>
#include <QtDebug>

finance::finance(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::finance)
{
    ui->setupUi(this);
}

finance::~finance()
{
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


        while (!file.atEnd()) {
            QByteArray line = file.readLine();
            QStringList wordList;
            wordList.append(line.split(',').first());
            int row = ui->noneTable->rowCount();
            ui->noneTable->insertRow(row);
            ui->noneTable->setItem(row, 0, new QTableWidgetItem(wordList.at(0)));
        }

        file.close();
    }
}
