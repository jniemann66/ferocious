/*
* Copyright (C) 2016 - 2021 Judd Niemann - All Rights Reserved.
* You may use, distribute and modify this code under the
* terms of the GNU Lesser General Public License, version 2.1
*
* You should have received a copy of GNU Lesser General Public License v2.1
* with this file. If not, please refer to: https://github.com/jniemann66/ferocious
*/

#include "converteroutputbox.h"

ConverterOutputBox::ConverterOutputBox(QWidget* parent) : QTextBrowser(parent) {}

void ConverterOutputBox::contextMenuEvent(QContextMenuEvent *e)
{
    auto m = createStandardContextMenu();
    m->addSeparator();
    m->addAction(tr("Clear"), [this]() {
        clear();
    });
    m->exec(e->globalPos());
    delete m;
}
