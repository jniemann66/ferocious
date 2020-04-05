/*
* Copyright (C) 2016 - 2020 Judd Niemann - All Rights Reserved.
* You may use, distribute and modify this code under the
* terms of the GNU Lesser General Public License, version 2.1
*
* You should have received a copy of GNU Lesser General Public License v2.1
* with this file. If not, please refer to: https://github.com/jniemann66/ferocious
*/

#include "cmdlinehighlighterdelegate.h"

#include <QPainter>
#include <QTextDocument>

CmdLineHighlighterDelegate::CmdLineHighlighterDelegate(QObject* parent) : QStyledItemDelegate(parent) {}

void CmdLineHighlighterDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (option.state & QStyle::State_Selected) {
        painter->fillRect( option.rect, option.palette.highlight());
    }

    QTextDocument document;
    document.setTextWidth(option.rect.width());
    QVariant value = index.data(Qt::DisplayRole);

    painter->save();
    if(value.isValid() && !value.isNull()) {
        document.setHtml(getHtmlHighlighting(value.toString()));
        painter->translate(QPoint{option.rect.left(), option.rect.top() + 4 /* to-do : how to calculate this properly ? */});
        document.drawContents(painter);
    }

    painter->restore();
}

QString CmdLineHighlighterDelegate::getHtmlHighlighting(const QString& input)
{
    const QString consoleGreen{"#66A334"};
    const QString consoleWhite{"#C0C0C0"};
    const QString consoleCyan{"#45C6D6"};
    const QString consoleYellow{"#D6C878"};
    const QString consoleAmber{"#D6953E"};
    const QString consoleRed{"#ff8080"};

    Q_UNUSED(consoleGreen);
    Q_UNUSED(consoleRed);

    QStringList tokens = input.split(" ");
    QStringList htmlTokens;
    for(const QString& token : tokens) {
        if(token.contains(QRegularExpression{"{.*}"})) { // enclosed in braces
             htmlTokens.append(QString{"<font color=\"%1\">%2</font>"}.arg(consoleAmber, token));
        }
        else if(token.contains(QRegularExpression{"--.*"})) { // double-hyphen option
             htmlTokens.append(QString{"<font color=\"%1\">%2</font>"}.arg(consoleCyan, token));
        }
        else if(token.contains(QRegularExpression{"-[^-]*"})) { // single-hyphen option
             htmlTokens.append(QString{"<font color=\"%1\">%2</font>"}.arg(consoleYellow, token));
        }
        else {
            htmlTokens.append(QString{"<font color=\"%1\">%2</font>"}.arg(consoleWhite, token));
        }
    }
    return htmlTokens.join(" ");
}
