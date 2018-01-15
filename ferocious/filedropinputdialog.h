#ifndef FILEDROPINPUTDIALOG_H
#define FILEDROPINPUTDIALOG_H

#include <QInputDialog>
#include <QDragEnterEvent>
#include <QDropEvent>

class FileDropInputDialog : public QInputDialog
{
public:
    FileDropInputDialog(QWidget *parent, Qt::WindowFlags flags);

protected:
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *event) override;
};

#endif // FILEDROPINPUTDIALOG_H
