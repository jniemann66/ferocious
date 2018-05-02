#include "converterdefinition.h"

void ConverterDefinition::fromJson(const QJsonObject &json)
{
    priority = json.value("priority").toInt();
    enabled = json.value("enabled").toBool();
    name = json.value("name").toString();
    comment = json.value("comment").toString();
    inputFileExt = json.value("inputfileext").toString();
    outputFileExt = json.value("outputfileext").toString();
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
