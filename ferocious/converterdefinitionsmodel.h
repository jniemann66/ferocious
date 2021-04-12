/*
* Copyright (C) 2016 - 2021 Judd Niemann - All Rights Reserved.
* You may use, distribute and modify this code under the
* terms of the GNU Lesser General Public License, version 2.1
*
* You should have received a copy of GNU Lesser General Public License v2.1
* with this file. If not, please refer to: https://github.com/jniemann66/ferocious
*/

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
