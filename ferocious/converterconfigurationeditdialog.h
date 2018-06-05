#ifndef CONVERTERCONFIGURATIONEDITDIALOG_H
#define CONVERTERCONFIGURATIONEDITDIALOG_H

#include "converterdefinition.h"

#include <QDialog>
#include <QObject>
#include <QLineEdit>
#include <QCheckBox>

class ConverterConfigurationEditDialog : public QDialog
{

public:
    ConverterConfigurationEditDialog(QWidget* parent);

private:
    QLineEdit* priorityEdit;
    QCheckBox* enabledCheckbox;
    QLineEdit* nameEdit;
    QLineEdit* commentEdit;
    QLineEdit* inputFileExtEdit;
    QLineEdit* outputFileExtEdit;
    QLineEdit* executableEdit;
    QLineEdit* executablePathEdit;
    QLineEdit* commandLineEdit;

};

#endif // CONVERTERCONFIGURATIONEDITDIALOG_H

/*
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
*/
