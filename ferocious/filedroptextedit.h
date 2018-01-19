#ifndef FILEDROPTEXTEDIT_H
#define FILEDROPTEXTEDIT_H

#include <QPlainTextEdit>
#include <QDropEvent>
#include <QDragEnterEvent>

class FileDropTextEdit : public QPlainTextEdit
{
public:
    FileDropTextEdit(QWidget* parent = nullptr);
protected:
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;
};

#endif // FILEDROPTEXTEDIT_H
