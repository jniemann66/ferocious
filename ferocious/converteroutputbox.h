/*
* Copyright (C) 2016 - 2021 Judd Niemann - All Rights Reserved.
* You may use, distribute and modify this code under the
* terms of the GNU Lesser General Public License, version 2.1
*
* You should have received a copy of GNU Lesser General Public License v2.1
* with this file. If not, please refer to: https://github.com/jniemann66/ferocious
*/

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
