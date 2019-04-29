#include "tablecontextmenu.h"

TableContextMenu::TableContextMenu() :
    permAction("Permanent", this) {
    permAction.setCheckable(true);
    addAction(&permAction);
}

void TableContextMenu::setContext(bool isPerm) {
    permAction.setChecked(isPerm);
}
