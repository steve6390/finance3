#include "financetablewidget.h"
#include <QDebug>
#include <QApplication>

FinanceTableWidget::FinanceTableWidget(QWidget* parent)
    : QTableWidget(parent) {
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
}

void FinanceTableWidget::keyPressEvent(QKeyEvent* event) {
    qDebug() << "FinanceTableWidget: Key press!\n";

    // Only keys with Alt modifier are interesting
    if(event->modifiers() & Qt::AltModifier) {
        switch(event->key()) {
        case Qt::Key_Left:
            emit moveLeft(this->currentRow());
            return;
        case Qt::Key_Right:
            emit moveRight(this->currentRow());
            return;
        default:
            break;
        }
    }

    // We must pass along unhandled events
    QTableWidget::keyPressEvent(event);
}

