#ifndef CHECKBOXDELEGATE_H
#define CHECKBOXDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>
#include <QPainter>

class CheckBoxDelegate : public QStyledItemDelegate
{
public:
    CheckBoxDelegate(QObject* parent = nullptr);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

};

#endif // CHECKBOXDELEGATE_H
