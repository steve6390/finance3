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

    // accepted flag is true by default, so clear it
    event->ignore();

    // Only keys with Alt modifier are interesting
    if(event->modifiers() & Qt::AltModifier) {
        switch(event->key()) {
        case Qt::Key_Left:
            emit moveLeft(this->currentRow());
            event->accept();
            break;
        case Qt::Key_Right:
            emit moveRight(this->currentRow());
            event->accept();
            break;
        default:
            break;
        }
    }

    // If we didn't handle this event, then pass it along.
    if(not event->isAccepted())
        QTableWidget::keyPressEvent(event);
}

