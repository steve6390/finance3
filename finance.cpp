#include "finance.h"
#include "helpers.h"
#include "ui_finance.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QStringList>
#include <QtDebug>
#include <QApplication>

finance::finance(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::finance)
{
    ui->setupUi(this);

    connect(ui->noneTable, &QTableWidget::cellClicked,
            this, &finance::noneTableClickEvent);

}

void finance::noneTableClickEvent(int row, int column) {
    qDebug() << "Clicked on noneTable (row,col) = " << row << "," << column << endl;
    if(override_stack.empty())
        return;

    QList<QTableWidgetItem*> rowList;

    switch(override_stack.back()) {
    case right:
        rowList = takeRow(ui->noneTable, row);
        addRow(ui->jointTable, rowList);
        break;
    case left:
        rowList = takeRow(ui->noneTable, row);
        addRow(ui->mineTable, rowList);
        break;
    default:
        break;
    }
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
            int row = ui->noneTable->rowCount();
            ui->noneTable->insertRow(row);

            ui->noneTable->setItem(row, 0, new QTableWidgetItem(wordList.at(0)));
            ui->noneTable->setItem(row, 1, new QTableWidgetItem(wordList.at(1)));
            if(wordList.at(4) == "credit")
                sign = "-";
            else
                sign = "";
            ui->noneTable->setItem(row, 2, new QTableWidgetItem(sign + wordList.at(3)));

            // DEBUG
            if(row > 30)
                break;
        }

        file.close();
    }
}

void finance::keyPressEvent(QKeyEvent* event) {
    if(override_stack.empty()) {
        switch(event->key()) {
        case Qt::Key_Control:
            qDebug() << "Ctrl press! " << endl;
            QApplication::setOverrideCursor(leftCursor);
            override_stack.push_back(left);
            break;
        case Qt::Key_Shift:
            qDebug() << "Shift press!";
            QApplication::setOverrideCursor(rightCursor);
            override_stack.push_back(right);
            break;
        default:
            qDebug() << "Unknown keypress!";
            event->ignore();
            break;
        }
    }
    qDebug() << endl;
}

void finance::keyReleaseEvent(QKeyEvent* event) {
    (void)event;
    qDebug() << "Release!";


    if(override_stack.empty()) {
        event->ignore();
    } else {
        QApplication::restoreOverrideCursor();
        override_stack.pop_back();
    }
}
