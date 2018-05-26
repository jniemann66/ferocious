#include "convertersmodel.h"

ConvertersModel::ConvertersModel(QObject* parent) : QAbstractTableModel(parent)
{

}

QVariant ConvertersModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::EditRole) return {};

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
