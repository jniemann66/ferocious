#ifndef FILEDROPTEXTEDIT_H
#define FILEDROPTEXTEDIT_H

// FileDropTextEdit - text editor that accepts filenames from drag and drop events

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
