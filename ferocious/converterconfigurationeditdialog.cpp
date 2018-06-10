#include "converterconfigurationeditdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

ConverterConfigurationEditDialog::ConverterConfigurationEditDialog(QWidget *parent) : QDialog(parent)
{
    // layouts
    auto mainLayout = new QVBoxLayout;
    auto fileExtenstionsLayout = new QHBoxLayout;
    auto inputFileExtLayout = new QVBoxLayout;
    auto outputFileExLayout = new QVBoxLayout;

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
    executablePathLabel = new QLabel("Path of Executable");
    QLabel* heading = new QLabel("Edit External Converter Settings");
    QLabel* priorityLabel = new QLabel("Priority");
    QLabel* nameLabel = new QLabel("Name");
    QLabel* commentLabel = new QLabel("Comments");
    QLabel* inputFileExtLabel = new QLabel("Input File Extension");
    QLabel* outputFileExtLabel = new QLabel("Output File Extension");
    QLabel* executableLabel = new QLabel("Executable");
    QLabel* commandLineLabel = new QLabel("Command Line Arguments");

    // set fonts
    QFont f = heading->font();
    QFont headingFont{f};
    headingFont.setPointSize(f.pointSize() + 2);
    heading->setFont(headingFont);

    // configure widgets
    heading->setAlignment(Qt::AlignHCenter);

    // hide things
    priorityLabel->setHidden(true);
    priorityEdit->setHidden(true);
    executableLabel->setHidden(true);
    executableEdit->setHidden(true);

    // attach
    mainLayout->addWidget(heading);
    mainLayout->addSpacing(12);
    mainLayout->addWidget(enabledCheckbox);
    mainLayout->addSpacing(6);
    mainLayout->addWidget(nameLabel);
    mainLayout->addWidget(nameEdit);
    mainLayout->addWidget(priorityLabel);
    mainLayout->addWidget(priorityEdit);
    inputFileExtLayout->addWidget(inputFileExtLabel);
    inputFileExtLayout->addWidget(inputFileExtEdit);
    outputFileExLayout->addWidget(outputFileExtLabel);
    outputFileExLayout->addWidget(outputFileExtEdit);
    fileExtenstionsLayout->addLayout(inputFileExtLayout);
    fileExtenstionsLayout->addLayout(outputFileExLayout);
    mainLayout->addLayout(fileExtenstionsLayout);
    mainLayout->addWidget(executableLabel);
    mainLayout->addWidget(executableEdit);
    mainLayout->addWidget(executablePathLabel);
    mainLayout->addWidget(executablePathEdit);
    mainLayout->addWidget(commandLineLabel);
    mainLayout->addWidget(commandLineEdit);
    mainLayout->addWidget(commentLabel);
    mainLayout->addWidget(commentEdit);
    mainLayout->addSpacing(12);
    mainLayout->addWidget(dialogButtonBox);
    mainLayout->addStretch();

    // set main Layout
    this->setLayout(mainLayout);

    setContentsMargins(12, 12, 12, 12);

    // connect signals / slots
    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

ConverterDefinition ConverterConfigurationEditDialog::getConverterDefinition() const
{
    ConverterDefinition converterDefinition;
    converterDefinition.priority = priorityEdit->text().toInt();
    converterDefinition.enabled = enabledCheckbox->isChecked();
    converterDefinition.name = nameEdit->text();
    converterDefinition.comment = commentEdit->text();
    converterDefinition.inputFileExt = inputFileExtEdit->text();
    converterDefinition.outputFileExt = outputFileExtEdit->text();
    converterDefinition.executable = executableEdit->text();
    converterDefinition.executablePath = executablePathEdit->text();
    converterDefinition.commandLine = commandLineEdit->text();
    return converterDefinition;
}

void ConverterConfigurationEditDialog::setConverterDefinition(const ConverterDefinition &converterDefinition)
{
    priorityEdit->setText(QString::number(converterDefinition.priority));
    enabledCheckbox->setChecked(converterDefinition.enabled);
    nameEdit->setText(converterDefinition.name);
    commentEdit->setText(converterDefinition.comment);
    inputFileExtEdit->setText(converterDefinition.inputFileExt);
    outputFileExtEdit->setText(converterDefinition.outputFileExt);
    executableEdit->setText(converterDefinition.executable);

    if(converterDefinition.executable.isEmpty()) {
        executablePathLabel->setText("Path of Executable");
    } else {
        executablePathLabel->setText(QString{"Path of Executable (%1)"}.arg(converterDefinition.executable));
    }

    executablePathEdit->setText(converterDefinition.executablePath);
    commandLineEdit->setText(converterDefinition.commandLine);
}
