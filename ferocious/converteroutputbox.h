#ifndef CONVERTEROUTPUTBOX_H
#define CONVERTEROUTPUTBOX_H

// ConverterOutputBox - widget for displaying output from converter

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
