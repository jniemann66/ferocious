#ifndef CONVERTERDEFINITIONSMODEL_H
#define CONVERTERDEFINITIONSMODEL_H

#include "converterdefinition.h"

#include <QAbstractTableModel>

class ConverterDefinitionsModel : public QAbstractTableModel
{
    QVector<ConverterDefinition> converterDefinitions;

public:
    ConverterDefinitionsModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &) const override;
    int columnCount(const QModelIndex &) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVector<ConverterDefinition> getConverterDefinitions() const;
    void setConverterDefinitions(const QVector<ConverterDefinition> &value);
};

#endif // CONVERTERDEFINITIONSMODEL_H
