#ifndef FILEDROPDIALOG_H
#define FILEDROPDIALOG_H

// FileDropDialog - a Dialog containing a text editor that accepts filenames from drag and drop events

#include "filedroptextedit.h"

#include <QDialog>
#include <QPlainTextEdit>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QString>
#include <QLabel>

class FileDropDialog : public QDialog
{
public:
    FileDropDialog(QWidget* parent = nullptr);
    void setText(const QString &title, const QString &label, const QString &text = QString());
    QString getText() const;

private:
    QLabel* textLabel;
    FileDropTextEdit* textEdit;

};

#endif // FILEDROPDIALOG_H