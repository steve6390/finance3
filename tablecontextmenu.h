#ifndef TABLECONTEXTMENU_H
#define TABLECONTEXTMENU_H

#include <QMenu>

class TableContextMenu : public QMenu
{
public:
    TableContextMenu();
    void setContext(bool isPerm);

private:
    QAction permAction;
};

#endif // TABLECONTEXTMENU_H
