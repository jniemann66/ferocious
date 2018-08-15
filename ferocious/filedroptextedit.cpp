#include "filedroptextedit.h"

#include <QMimeData>
#include <QDir>
#include <QDebug>

FileDropTextEdit::FileDropTextEdit(QWidget *parent) : QPlainTextEdit (parent)
{
     setAcceptDrops(true);
}

void FileDropTextEdit::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->hasText()) {
        e->acceptProposedAction();
    }
}

void FileDropTextEdit::dropEvent(QDropEvent *e)
{
    QStringList urlStrings = e->mimeData()->text().split("\n");
    QUrl url;
    QStringList paths;
    for(const auto& urlString : urlStrings) {
        if(urlString.isEmpty())
            continue;
        url = urlString;
        QString path = QDir::toNativeSeparators(url.path());

#ifdef Q_OS_WIN
        if(path.startsWith('\\')) {
            path.remove(0, 1);
        }
#endif

        paths.append(path);
    }

    QString txt = this->toPlainText();

    // if current text doesn't end in newline, add newline ...
    if(!txt.isEmpty() && txt.right(1) != '\n')
        insertPlainText("\n");

    if(!paths.isEmpty())
        insertPlainText(paths.join("\n"));
}
