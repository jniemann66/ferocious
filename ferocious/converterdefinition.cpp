#include "converterdefinition.h"

#include <QFile>
#include <QDebug>
#include <QJsonDocument>

// todo: add APE definition in defaults

void ConverterDefinition::fromJson(const QJsonObject &json)
{
    priority = json.value("priority").toInt();
    enabled = json.value("enabled").toBool();
    name = json.value("name").toString();
    comment = json.value("comment").toString();
    inputFileExt = json.value("inputfileext").toString();
    outputFileExt = json.value("outputfileext").toString();
    executable = json.value("executablename").toString();
    executablePath = json.value("executablepath").toString();
    commandLine = json.value("commandline").toString();

    downloadLocations.clear();
    QJsonArray dlArray = json.value("downloadlocations").toArray();
    for(const QJsonValue& dl : dlArray) {
        downloadLocations.append(dl.toString());
    }

    operatingSystems.clear();
    QJsonArray osArray = json.value("operatingsystems").toArray();
    for(const QJsonValue& os : osArray) {
        operatingSystems.append(os.toString());
    }
}

QJsonObject ConverterDefinition::toJson() const
{
    QJsonObject json;
    json.insert("priority", priority);
    json.insert("enabled", enabled);
    json.insert("name", name);
    json.insert("comment", comment);
    json.insert("inputfileext", inputFileExt);
    json.insert("outputfileext", outputFileExt);
    json.insert("executablename", executable);
    json.insert("executablepath", executablePath);
    json.insert("commandline", commandLine);

    QJsonArray dlArray = QJsonArray::fromStringList(downloadLocations);
    json.insert("downloadlocations", dlArray);

    QJsonArray osArray = QJsonArray::fromStringList(operatingSystems);
    json.insert("operatingsystems", osArray);

    return json;
}

bool ConverterDefinition::operator==(const ConverterDefinition &other)
{
    return priority == other.priority &&
    enabled == other.enabled &&
    name == other.name &&
    comment == other.comment &&
    inputFileExt == other.inputFileExt &&
    outputFileExt == other.outputFileExt &&
    executablePath == other.executablePath &&
    commandLine == other.commandLine &&
    downloadLocations == other.downloadLocations &&
    operatingSystems == other.operatingSystems;
}

QVector<ConverterDefinition> ConverterDefinition::loadConverterDefinitions(const QString& fileName) {
    QVector<ConverterDefinition> converterDefinitions; // retval
    QFile jsonFile(fileName);
    QDebug dbg = qDebug();
    dbg.noquote() << "Reading converter definitions from" << fileName << "...";

    if(jsonFile.open(QFile::ReadOnly)) {
        QJsonDocument d = QJsonDocument::fromJson(jsonFile.readAll());
        if(d.isArray()) {
            dbg << "success.";
            converterDefinitions.clear();
            int i = 0;
            for(const QJsonValue& v : d.array()) {
                ConverterDefinition c;
                c.fromJson(v.toObject());
                c.priority = i++;
#if defined(Q_OS_WIN)
                if(c.operatingSystems.contains("win", Qt::CaseInsensitive))
#elif defined(Q_OS_LINUX)
                if(c.operatingSystems.contains("linux", Qt::CaseInsensitive))
#elif defined(Q_OS_MACOS)
                if(c.operatingSystems.contains("macos", Qt::CaseInsensitive))
#endif
                {
                    converterDefinitions.append(c);
                }
            }
        }
    } else {
        dbg << "failed.";
    }
    return converterDefinitions;
}

void ConverterDefinition::saveConverterDefinitions(const QString& fileName, const QVector<ConverterDefinition>& converterDefinitions) {
    QJsonArray a;
    for(const ConverterDefinition& converterDefinition: converterDefinitions) {
        a.append(converterDefinition.toJson());
    }
    QFile jsonFile(fileName);
    jsonFile.open(QFile::WriteOnly);
    QJsonDocument d(a);

    QDebug dbg = qDebug();
    dbg.noquote() << "Writing converter definitions to" << fileName << "...";
    if(jsonFile.write(d.toJson()) == -1) {
        dbg << "failed.";
    } else {
        dbg << "success.";
    }
}
