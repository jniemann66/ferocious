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
