/*
* Copyright (C) 2016 - 2021 Judd Niemann - All Rights Reserved.
* You may use, distribute and modify this code under the
* terms of the GNU Lesser General Public License, version 2.1
*
* You should have received a copy of GNU Lesser General Public License v2.1
* with this file. If not, please refer to: https://github.com/jniemann66/ferocious
*/

#ifndef CMDLINEHIGHLIGHTERDELEGATE_H
#define CMDLINEHIGHLIGHTERDELEGATE_H

#include <QStyledItemDelegate>

class CmdLineHighlighterDelegate : public QStyledItemDelegate
{

public:
    CmdLineHighlighterDelegate(QObject *parent);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    static QString getHtmlHighlighting(const QString &input);
};

#endif // CMDLINEHIGHLIGHTERDELEGATE_H
