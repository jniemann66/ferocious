/*
* Copyright (C) 2016 - 2026 Judd Niemann - All Rights Reserved.
* You may use, distribute and modify this code under the
* terms of the GNU Lesser General Public License, version 2.1
*
* You should have received a copy of GNU Lesser General Public License v2.1
* with this file. If not, please refer to: https://github.com/jniemann66/ferocious
*/

#include "filedroptextedit.h"

#include <QMimeData>
#include <QDir>

FileDropTextEdit::FileDropTextEdit(QWidget *parent)
    : QPlainTextEdit (parent)
{
     setAcceptDrops(true);
}

void FileDropTextEdit::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasText()) {
        e->acceptProposedAction();
    }
}

void FileDropTextEdit::dropEvent(QDropEvent *e)
{
	const QStringList urlStrings = e->mimeData()->text().split("\n");
    QUrl url;
    QStringList paths;
    for (const auto& urlString : urlStrings) {
        if (urlString.isEmpty())
            continue;
        url = urlString;
        QString path = QDir::toNativeSeparators(url.path());

#ifdef Q_OS_WIN
        if (path.startsWith('\\')) {
            path.remove(0, 1);
        }
#endif

        paths.append(path);
    }

    QString txt = this->toPlainText();

    // if current text doesn't end in newline, add newline ...
    if (!txt.isEmpty() && txt.right(1) != '\n') {
        insertPlainText("\n");
    }

    if (!paths.isEmpty()) {
        insertPlainText(paths.join("\n"));
    }
}
