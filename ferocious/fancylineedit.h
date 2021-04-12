/*
* Copyright (C) 2016 - 2021 Judd Niemann - All Rights Reserved.
* You may use, distribute and modify this code under the
* terms of the GNU Lesser General Public License, version 2.1
*
* You should have received a copy of GNU Lesser General Public License v2.1
* with this file. If not, please refer to: https://github.com/jniemann66/ferocious
*/

#ifndef FANCYLINEEDIT_H
#define FANCYLINEEDIT_H

// FancyLineEdit: a QLineEdit with added Edit (...) and clear (x) buttons.
// The edit button opens a multi-line edit box.
// The line edit and the "expanded" multi-edit both accept the filenames of
// dragged-and-dropped files

#include "filedropdialog.h"

#include <QLineEdit>
#include <QDropEvent>
#include <QMimeData>

class QToolButton;

class FancyLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    FancyLineEdit(QWidget *parent = nullptr);
    ~FancyLineEdit() override;

    void hideEditButton();
    void hideClearButton();
    void showEditButton();
    void showClearButton();

protected:
    void resizeEvent(QResizeEvent *) override;
    void dragEnterEvent(QDragEnterEvent* e) override;
    void dropEvent(QDropEvent* e) override;

private:
    QToolButton *editButton;
    QToolButton *clearButton;
    FileDropDialog* fileDropDialog;

private slots:
    void on_editButton_Clicked();
};

#endif // FANCYLINEEDIT_H
