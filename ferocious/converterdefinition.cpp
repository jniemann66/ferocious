#include "converterdefinition.h"

ConverterDefinition::ConverterDefinition()
{

}

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

    QJsonArray dlArray;
    dlArray.fromStringList(downloadLocations);
    json.insert("downloadlocations", dlArray);

    QJsonArray osArray;
    osArray.fromStringList(operatingSystems);
    json.insert("operatingsystems", osArray);

    return json;
}

QString ConverterDefinition::getName() const
{
    return name;
}

void ConverterDefinition::setName(const QString &value)
{
    name = value;
}

QString ConverterDefinition::getInputFileExt() const
{
    return inputFileExt;
}

void ConverterDefinition::setInputFileExt(const QString &value)
{
    inputFileExt = value;
}

QString ConverterDefinition::getOutputFileExt() const
{
    return outputFileExt;
}

void ConverterDefinition::setOutputFileExt(const QString &value)
{
    outputFileExt = value;
}

QString ConverterDefinition::getExecutablePath() const
{
    return executablePath;
}

void ConverterDefinition::setExecutablePath(const QString &value)
{
    executablePath = value;
}

QString ConverterDefinition::getCommandLine() const
{
    return commandLine;
}

void ConverterDefinition::setCommandLine(const QString &value)
{
    commandLine = value;
}

QStringList ConverterDefinition::getDownloadLocations() const
{
    return downloadLocations;
}

void ConverterDefinition::setDownloadLocations(const QStringList &value)
{
    downloadLocations = value;
}

QStringList ConverterDefinition::getOperatingSystems() const
{
    return operatingSystems;
}

void ConverterDefinition::setOperatingSystems(const QStringList &value)
{
    operatingSystems = value;
}



bool ConverterDefinition::getEnabled() const
{
    return enabled;
}

void ConverterDefinition::setEnabled(bool value)
{
    enabled = value;
}

int ConverterDefinition::getPriority() const
{
    return priority;
}

void ConverterDefinition::setPriority(int value)
{
    priority = value;
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
