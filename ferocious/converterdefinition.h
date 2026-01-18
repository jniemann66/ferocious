/*
* Copyright (C) 2016 - 2026 Judd Niemann - All Rights Reserved.
* You may use, distribute and modify this code under the
* terms of the GNU Lesser General Public License, version 2.1
*
* You should have received a copy of GNU Lesser General Public License v2.1
* with this file. If not, please refer to: https://github.com/jniemann66/ferocious
*/

#ifndef CONVERTERDEFINITION_H
#define CONVERTERDEFINITION_H

// converterdefinition.h - Type for defining properties of external file converters

#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>

struct ConverterDefinition
{
    int priority{};
	bool enabled{false};
    QString name;
    QString comment;
    QString inputFileExt;
    QString outputFileExt;
    QString executable;
    QString executablePath;
    QString commandLine;
    QStringList downloadLocations;
    QStringList operatingSystems;

    void fromJson(const QJsonObject& json);
    QJsonObject toJson() const;
    bool operator==(const ConverterDefinition& other);

public:
    static QVector<ConverterDefinition> loadConverterDefinitions(const QString &fileName);
    static void saveConverterDefinitions(const QString &fileName, const QVector<ConverterDefinition> &converterDefinitions);
};

#endif // CONVERTERDEFINITION_H
