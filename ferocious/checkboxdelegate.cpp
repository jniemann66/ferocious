#include "checkboxdelegate.h"

#include <QApplication>

CheckBoxDelegate::CheckBoxDelegate(QObject *parent) : QStyledItemDelegate(parent), scale(1.1) {}

void CheckBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    QRect rect(option.rect.left() / scale, option.rect.top() / scale, option.rect.width() / scale, option.rect.height() / scale);
    painter->save();
    painter->scale(scale, scale);

    if (option.state & QStyle::State_Selected)
        painter->fillRect(rect, option.palette.highlight());

    QStyleOptionButton checkBoxOptions;
    checkBoxOptions.rect = rect;
    checkBoxOptions.state |= (index.data().toBool() ? QStyle::State_On : QStyle::State_Off);
    QApplication::style()->drawControl(QStyle::CE_CheckBox, &checkBoxOptions, painter);
    painter->restore();
}

bool CheckBoxDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonRelease)
    {
        model->setData(index, !index.data().toBool());  // toggle checkbox state
        return true;
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

qreal CheckBoxDelegate::getScale() const
{
    return scale;
}

void CheckBoxDelegate::setScale(const qreal &value)
{
    scale = value;
}
