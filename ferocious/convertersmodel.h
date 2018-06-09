#ifndef CONVERTERSMODEL_H
#define CONVERTERSMODEL_H

#include "converterdefinition.h"

#include <QAbstractTableModel>

class ConvertersModel : public QAbstractTableModel
{
    QVector<ConverterDefinition> converterDefinitions;

public:
    ConvertersModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &) const override;
    int columnCount(const QModelIndex &) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVector<ConverterDefinition> getConverterDefinitions() const;
    void setConverterDefinitions(const QVector<ConverterDefinition> &value);
};

#endif // CONVERTERSMODEL_H
