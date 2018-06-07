#include "converterconfigurationeditdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

ConverterConfigurationEditDialog::ConverterConfigurationEditDialog(QWidget *parent) : QDialog(parent)
{
    // layouts
    auto mainLayout = new QVBoxLayout;

    // widgets
    enabledCheckbox = new QCheckBox("Enabled");
    priorityEdit = new QLineEdit;
    nameEdit = new QLineEdit;
    commentEdit = new QLineEdit;
    inputFileExtEdit = new QLineEdit;
    outputFileExtEdit = new QLineEdit;
    executableEdit = new QLineEdit;
    executablePathEdit = new QLineEdit;
    commandLineEdit = new QLineEdit;
    dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    // labels
    QLabel* priorityLabel = new QLabel("Priority");
    QLabel* nameLabel = new QLabel("Name");
    QLabel* commentLabel = new QLabel("Comments");
    QLabel* inputFileExtLabel = new QLabel("Input File Extension");
    QLabel* outputFileExtLabel = new QLabel("Output File Extension");
    QLabel* executableLabel = new QLabel("Executable");
    QLabel* executablePathLabel = new QLabel("Executable Path");
    QLabel* commandLineLabel = new QLabel("Command Line Arguments");

    // attach
    mainLayout->addWidget(priorityLabel);
    mainLayout->addWidget(priorityEdit);
    mainLayout->addWidget(nameLabel);
    mainLayout->addWidget(nameEdit);
    mainLayout->addWidget(commentLabel);
    mainLayout->addWidget(commentEdit);
    mainLayout->addWidget(inputFileExtLabel);
    mainLayout->addWidget(inputFileExtEdit);
    mainLayout->addWidget(outputFileExtLabel);
    mainLayout->addWidget(outputFileExtEdit);
    mainLayout->addWidget(executableLabel);
    mainLayout->addWidget(executableEdit);
    mainLayout->addWidget(executablePathLabel);
    mainLayout->addWidget(executablePathEdit);
    mainLayout->addWidget(commandLineLabel);
    mainLayout->addWidget(commandLineEdit);
    mainLayout->addWidget(dialogButtonBox);

    // set main Layout
    this->setLayout(mainLayout);

    // connect signals / slots
    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

ConverterDefinition ConverterConfigurationEditDialog::getConverterDefinition() const
{
    return converterDefinition;
}

void ConverterConfigurationEditDialog::setConverterDefinition(const ConverterDefinition &value)
{
    converterDefinition = value;

    priorityEdit->setText(QString::number(converterDefinition.priority));
    enabledCheckbox->setChecked(converterDefinition.enabled);
    nameEdit->setText(converterDefinition.name);
    commentEdit->setText(converterDefinition.comment);
    inputFileExtEdit->setText(converterDefinition.inputFileExt);
    outputFileExtEdit->setText(converterDefinition.outputFileExt);
    executableEdit->setText(converterDefinition.executable);
    executablePathEdit->setText(converterDefinition.executablePath);
    commandLineEdit->setText(converterDefinition.commandLine);
}
