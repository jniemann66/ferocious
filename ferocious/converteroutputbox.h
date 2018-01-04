#ifndef CONVERTEROUTPUTBOX_H
#define CONVERTEROUTPUTBOX_H

#include <QTextBrowser>
#include <QContextMenuEvent>
#include <QMenu>

class ConverterOutputBox : public QTextBrowser
{
public:
    ConverterOutputBox(QWidget* parent = nullptr);
protected:
    void contextMenuEvent(QContextMenuEvent* e) override;
};

#endif // CONVERTEROUTPUTBOX_H
