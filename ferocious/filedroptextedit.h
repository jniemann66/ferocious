/*
* Copyright (C) 2016 - 2026 Judd Niemann - All Rights Reserved.
* You may use, distribute and modify this code under the
* terms of the GNU Lesser General Public License, version 2.1
*
* You should have received a copy of GNU Lesser General Public License v2.1
* with this file. If not, please refer to: https://github.com/jniemann66/ferocious
*/

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
