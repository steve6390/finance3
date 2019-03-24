#ifndef FINANCETABLEWIDGET_H
#define FINANCETABLEWIDGET_H

#include <QTableWidget>
#include <QKeyEvent>

class FinanceTableWidget : public QTableWidget
{
Q_OBJECT
public:
    FinanceTableWidget(QWidget* parent = 0);
protected:
    void keyPressEvent(QKeyEvent* event);

signals:
    void moveLeft(int row);
    void moveRight(int row);
};

#endif // FINANCETABLEWIDGET_H
