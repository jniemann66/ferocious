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
    int priority;
    bool enabled;
    QString name;
    QString comment;
    QString inputFileExt;
    QString outputFileExt;
    QString executable;
    QString executablePath;
    QString commandLine;
    QStringList downloadLocations;
    QStringList operatingSystems;

    // json services
    void fromJson(const QJsonObject& json);
    QJsonObject toJson() const;

    bool operator==(const ConverterDefinition& other);

public:
    static QVector<ConverterDefinition> loadConverterDefinitions(const QString &fileName);
    static void saveConverterDefinitions(const QString &fileName, const QVector<ConverterDefinition> &converterDefinitions);
};

#endif // CONVERTERDEFINITION_H
