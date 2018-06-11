#ifndef CONVERTERCONFIGURATIONEDITDIALOG_H
#define CONVERTERCONFIGURATIONEDITDIALOG_H

#include "converterdefinition.h"
#include "fancylineedit.h"

#include <QDialog>
#include <QObject>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QPushButton>

class ConverterConfigurationEditDialog : public QDialog
{

public:
    ConverterConfigurationEditDialog(QWidget* parent);

    ConverterDefinition getConverterDefinition() const;
    void setConverterDefinition(const ConverterDefinition &converterDefinition);

private:
    QLineEdit* priorityEdit;
    QCheckBox* enabledCheckbox;
    QLineEdit* nameEdit;
    QLineEdit* commentEdit;
    QLineEdit* inputFileExtEdit;
    QLineEdit* outputFileExtEdit;
    QLineEdit* executableEdit;
    QLabel* executablePathLabel;
    FancyLineEdit* executablePathEdit;
    QPushButton* executablePathBrowseButton;
    QLineEdit* commandLineEdit;
    QDialogButtonBox* dialogButtonBox;

};

#endif // CONVERTERCONFIGURATIONEDITDIALOG_H

