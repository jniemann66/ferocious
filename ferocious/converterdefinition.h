#ifndef CONVERTERDEFINITION_H
#define CONVERTERDEFINITION_H

// converterdefinition.h - Type for defining properties of external file converters

#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>

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
    bool getEnabled() const;
    void setEnabled(bool value);
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
    QStringList getDownloadLocations() const;
    void setDownloadLocations(const QStringList &value);
    QStringList getOperatingSystems() const;
    void setOperatingSystems(const QStringList &value);

private:

    bool operator==(const ConverterDefinition& other);

    int priority;
    bool enabled;
    QString name;
    QString comment;
    QString inputFileExt;
    QString outputFileExt;
    QString executablePath;
    QString commandLine;
    QStringList downloadLocations;
    QStringList operatingSystems;
};

#endif // CONVERTERDEFINITION_H
