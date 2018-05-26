#ifndef CONVERTERSMODEL_H
#define CONVERTERSMODEL_H

#include "converterdefinition.h"

#include <QAbstractTableModel>

class ConvertersModel : public QAbstractTableModel
{
    QList<ConverterDefinition> converterDefinitions;

public:
    ConvertersModel(QObject *parent);
    QVariant data(const QModelIndex &index, int role) const override;


};

#endif // CONVERTERSMODEL_H
