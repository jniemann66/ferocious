#ifndef CHECKBOXDELEGATE_H
#define CHECKBOXDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>
#include <QPainter>

class CheckBoxDelegate : public QStyledItemDelegate
{

public:
    CheckBoxDelegate(QObject* parent = nullptr);
    qreal getScale() const;
    void setScale(const qreal &value);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

private:
    qreal scale;

};

#endif // CHECKBOXDELEGATE_H
