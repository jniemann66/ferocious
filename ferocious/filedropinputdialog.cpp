#include "filedropinputdialog.h"

#include <QMimeData>
#include <QDir>

FileDropInputDialog::FileDropInputDialog(QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::WindowFlags()) : QInputDialog(parent, flags)
{
     setAcceptDrops(true);
}

void FileDropInputDialog::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->hasText()) {
        e->acceptProposedAction();
    }
}

void FileDropInputDialog::dropEvent(QDropEvent *e)
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
        if(path.startsWith('\\')){
            path.remove(0,1);
        }
    #endif

        paths.append(path);

    }

    if(!paths.isEmpty())
        setTextValue(paths.join("\n"));
}

