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
