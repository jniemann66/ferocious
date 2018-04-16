#include "converteroutputbox.h"

ConverterOutputBox::ConverterOutputBox(QWidget* parent) : QTextBrowser(parent) {}

void ConverterOutputBox::contextMenuEvent(QContextMenuEvent *e)
{
    auto m = createStandardContextMenu();
    m->addSeparator();
    m->addAction("Clear", [this]() {
        clear();
    });
    m->exec(e->globalPos());
    delete m;
}
