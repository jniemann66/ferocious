#include "checkboxdelegate.h"
#include <QApplication>

CheckBoxDelegate::CheckBoxDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

void CheckBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    }
    QStyleOptionButton cbIndicator;
    cbIndicator.rect = option.rect;
    bool isChecked = index.data().toBool();
    cbIndicator.state = QStyle::State_Enabled | (isChecked ? QStyle::State_On : QStyle::State_Off);
    QApplication::style()->drawControl(QStyle::CE_CheckBox, &cbIndicator, painter);
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
