/*
* Copyright (C) 2016 - 2026 Judd Niemann - All Rights Reserved.
* You may use, distribute and modify this code under the
* terms of the GNU Lesser General Public License, version 2.1
*
* You should have received a copy of GNU Lesser General Public License v2.1
* with this file. If not, please refer to: https://github.com/jniemann66/ferocious
*/

#include "checkboxdelegate.h"
#include <QApplication>

CheckBoxDelegate::CheckBoxDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void CheckBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    }

    // Center the indicator in the cell
    const int iw = cb.style()->pixelMetric(QStyle::PM_IndicatorWidth, nullptr, &cb);
    const int ih = cb.style()->pixelMetric(QStyle::PM_IndicatorHeight, nullptr, &cb);
    const QRect indicatorRect(
        option.rect.left() + (option.rect.width() - iw) / 2,
        option.rect.top() + (option.rect.height() - ih) / 2,
        iw, ih
    );

    QStyleOptionButton opt;
    opt.initFrom(&cb);
    opt.rect = indicatorRect;
    opt.state = QStyle::State_Enabled | (index.data().toBool() ? QStyle::State_On : QStyle::State_Off);
    cb.style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &opt, painter, &cb);

    painter->restore();
}

bool CheckBoxDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        model->setData(index, !index.data().toBool());  // toggle checkbox state
        return true;
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
