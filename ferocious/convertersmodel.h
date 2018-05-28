#ifndef CONVERTERSMODEL_H
#define CONVERTERSMODEL_H

#include "converterdefinition.h"

#include <QAbstractTableModel>

class ConvertersModel : public QAbstractTableModel
{
    QList<ConverterDefinition> converterDefinitions;

public:
    ConvertersModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &) const override { return converterDefinitions.count(); }
    int columnCount(const QModelIndex &) const override { return 11; }
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role) const override;
};

#endif // CONVERTERSMODEL_H
