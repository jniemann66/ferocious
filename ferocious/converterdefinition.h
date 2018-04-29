#ifndef CONVERTERDEFINITION_H
#define CONVERTERDEFINITION_H

// converterdefinition.h - Type for defining properties of external file converters

#include <QString>
#include <QJsonObject>

class ConverterDefinition
{
public:
    ConverterDefinition();

    // json services
    void fromJson(const QJsonObject& json);
    QJsonObject toJson() const;

    // getters & setters
    int getPriority() const;
    void setPriority(int value);
    QString getName() const;
    void setName(const QString &value);
    QString getInputFileExt() const;
    void setInputFileExt(const QString &value);
    QString getOutputFileExt() const;
    void setOutputFileExt(const QString &value);
    QString getExecutablePath() const;
    void setExecutablePath(const QString &value);
    QString getCommandLine() const;
    void setCommandLine(const QString &value);

private:
    int priority;
    QString name;
    QString comment;
    QString inputFileExt;
    QString outputFileExt;
    QString executablePath;
    QString commandLine;
};

#endif // CONVERTERDEFINITION_H
