/*
* Copyright (C) 2016 - 2023 Judd Niemann - All Rights Reserved.
* You may use, distribute and modify this code under the
* terms of the GNU Lesser General Public License, version 2.1
*
* You should have received a copy of GNU Lesser General Public License v2.1
* with this file. If not, please refer to: https://github.com/jniemann66/ferocious
*/

#include "converterdefinitionsmodel.h"

ConverterDefinitionsModel::ConverterDefinitionsModel(QObject* parent) : QAbstractTableModel(parent) {}

int ConverterDefinitionsModel::rowCount(const QModelIndex &) const
{
    return converterDefinitions.count();
}

int ConverterDefinitionsModel::columnCount(const QModelIndex &) const
{
    return 11;
}

QVariant ConverterDefinitionsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return {};
    }

    switch (section) {
    case 0: return tr("Priority");
    case 1: return tr("Enabled");
    case 2: return tr("Name");
    case 3: return tr("Comment");
    case 4: return tr("Input Type");
    case 5: return tr("Output Type");
    case 6: return tr("Executable");
    case 7: return tr("Executable Path");
    case 8: return tr("Command Line");
    case 9: return tr("Download Locations");
    case 10: return tr("Operating Systems");
    default: return {};
    }
}

QVariant ConverterDefinitionsModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::TextAlignmentRole) {
        switch(index.column()) {
        case 0: return Qt::AlignCenter;
        case 1: return Qt::AlignVCenter;
        case 2: return Qt::AlignVCenter;
        case 3: return Qt::AlignVCenter;
        case 4: return Qt::AlignCenter;
        case 5: return Qt::AlignCenter;
        case 6: return Qt::AlignVCenter;
        case 7: return Qt::AlignVCenter;
        case 8: return Qt::AlignVCenter;
        case 9: return Qt::AlignVCenter;
        case 10: return Qt::AlignCenter;
        default: return Qt::AlignVCenter;// QAbstractTableModel::data(index, role, Qt::TextAlignmentRole);
        };
    }

    else if (role == Qt::DisplayRole || role == Qt::EditRole) {
        const auto& converter = converterDefinitions.at(index.row());
        switch(index.column()) {
        case 0: return converter.priority;
        case 1: return converter.enabled;
        case 2: return converter.name;
        case 3: return converter.comment;
        case 4: return converter.inputFileExt;
        case 5: return converter.outputFileExt;
        case 6: return converter.executable;
        case 7: return converter.executablePath;
        case 8: return converter.commandLine;
        case 9: return converter.downloadLocations;
        case 10: return converter.operatingSystems;
        default: return {};
        };
    }

    else {
        return {};
    }
}

bool ConverterDefinitionsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role == Qt::EditRole) {
        switch(index.column()) {
        case 1:
            converterDefinitions[index.row()].enabled = value.toBool();
            emit dataChanged(index, index);
            return true;

        default:
            {
            }
        }
    }
    return false;
}

QVector<ConverterDefinition> ConverterDefinitionsModel::getConverterDefinitions() const
{
    return converterDefinitions;
}

void ConverterDefinitionsModel::setConverterDefinitions(const QVector<ConverterDefinition> &value)
{
    converterDefinitions = value;
    emit rowsInserted({}, 0, converterDefinitions.count() - 1, {});
}
