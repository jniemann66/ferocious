#include "converterdefinition.h"

ConverterDefinition::ConverterDefinition()
{

}

void ConverterDefinition::fromJson(const QJsonObject &json)
{

}

QJsonObject ConverterDefinition::toJson() const
{

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

int ConverterDefinition::getPriority() const
{
    return priority;
}

void ConverterDefinition::setPriority(int value)
{
    priority = value;
}
