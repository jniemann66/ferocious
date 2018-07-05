#include "converterconfigurationeditdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDesktopServices>

ConverterConfigurationEditDialog::ConverterConfigurationEditDialog(QWidget *parent) : QDialog(parent), showToolTips(true)
{
    // layouts
    auto mainLayout = new QVBoxLayout;
    auto fileExtenstionsLayout = new QHBoxLayout;
    auto inputFileExtLayout = new QVBoxLayout;
    auto outputFileExtLayout = new QVBoxLayout;
    auto executablePathLayout = new QHBoxLayout;
    auto downloadLocationLayout = new QHBoxLayout;

    // Widgets
    enabledCheckbox = new QCheckBox(tr("Enabled"));
    priorityEdit = new QLineEdit;
    nameEdit = new QLineEdit;
    commentEdit = new QTextEdit;
    downloadLocationEdit = new QLineEdit;
    openURLButton = new QPushButton(tr("Launch"));
    inputFileExtEdit = new QLineEdit;
    outputFileExtEdit = new QLineEdit;
    executableEdit = new QLineEdit;
    executablePathEdit = new FancyLineEdit;
    executablePathBrowseButton = new QPushButton(tr("Browse ..."));
    commandLineEdit = new QLineEdit;
    dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    // labels
    executablePathLabel = new QLabel(tr("Path of Executable"));
    QLabel* heading = new QLabel(tr("Edit External Converter Settings"));
    QLabel* priorityLabel = new QLabel(tr("Priority"));
    QLabel* nameLabel = new QLabel(tr("Name"));
    QLabel* commentLabel = new QLabel(tr("Comments"));
    QLabel* downloadLocationLabel = new QLabel(tr("Link to Download / Information"));
    QLabel* inputFileExtLabel = new QLabel(tr("Input File Extension"));
    QLabel* outputFileExtLabel = new QLabel(tr("Output File Extension"));
    QLabel* executableLabel = new QLabel(tr("Executable"));
    QLabel* commandLineLabel = new QLabel(tr("Command Line Arguments"));

    // set fonts
    QFont f = heading->font();
    QFont headingFont{f};
    headingFont.setPointSize(f.pointSize() + 2);
    heading->setFont(headingFont);

    // configure widgets
    heading->setAlignment(Qt::AlignHCenter);
    executablePathEdit->hideEditButton();
    QFontMetrics m(commentEdit->font());
    commentEdit->setMaximumHeight(10 * m.lineSpacing());

    // set tooltips
    executablePathEdit->setToolTip(tr("Filename of actual converter program"));
    openURLButton->setToolTip(tr("Open link in Browser"));
    commandLineEdit->setToolTip(tr("Use {i} for Input File and {o} for Output File"));

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
    outputFileExtLayout->addWidget(outputFileExtLabel);
    outputFileExtLayout->addWidget(outputFileExtEdit);
    fileExtenstionsLayout->addLayout(inputFileExtLayout);
    fileExtenstionsLayout->addLayout(outputFileExtLayout);
    mainLayout->addLayout(fileExtenstionsLayout);
    mainLayout->addWidget(executableLabel);
    mainLayout->addWidget(executableEdit);
    mainLayout->addWidget(executablePathLabel);
    executablePathLayout->addWidget(executablePathEdit);
    executablePathLayout->addWidget(executablePathBrowseButton);
    mainLayout->addLayout(executablePathLayout);
    mainLayout->addWidget(commandLineLabel);
    mainLayout->addWidget(commandLineEdit);
    mainLayout->addWidget(commentLabel);
    mainLayout->addWidget(commentEdit);
    mainLayout->addWidget(downloadLocationLabel);
    downloadLocationLayout->addWidget(downloadLocationEdit);
    downloadLocationLayout->addWidget(openURLButton);
    mainLayout->addLayout(downloadLocationLayout);
    mainLayout->addSpacing(12);
    mainLayout->addStretch();
    mainLayout->addWidget(dialogButtonBox);

    // set main Layout
    this->setLayout(mainLayout);

    setContentsMargins(12, 12, 12, 12);

    // connect signals / slots
    connect(executablePathBrowseButton, &QPushButton::clicked, this, [this] {
       promptForExecutableLocation();
    });

    connect(openURLButton, &QPushButton::clicked, this, [this] {
       QDesktopServices::openUrl(QUrl(downloadLocationEdit->text()));
    });

    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

ConverterDefinition ConverterConfigurationEditDialog::getConverterDefinition() const
{
    ConverterDefinition converterDefinition;
    converterDefinition.priority = priorityEdit->text().toInt();
    converterDefinition.enabled = enabledCheckbox->isChecked();
    converterDefinition.name = nameEdit->text();
    converterDefinition.comment = commentEdit->document()->toPlainText();
    converterDefinition.downloadLocations = QStringList{downloadLocationEdit->text()};
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
    commentEdit->setPlainText(converterDefinition.comment);
    if(!converterDefinition.downloadLocations.isEmpty())
        downloadLocationEdit->setText(converterDefinition.downloadLocations.first());
    inputFileExtEdit->setText(converterDefinition.inputFileExt);
    outputFileExtEdit->setText(converterDefinition.outputFileExt);
    executableEdit->setText(converterDefinition.executable);

    if(converterDefinition.executable.isEmpty()) {
        executablePathLabel->setText(tr("Path of Executable"));
    } else {
        executablePathLabel->setText(QString(tr("Path of Executable (%1)")).arg(converterDefinition.executable));
    }

    executablePathEdit->setText(converterDefinition.executablePath);
    commandLineEdit->setText(converterDefinition.commandLine);
}

void ConverterConfigurationEditDialog::setShowToolTips(bool value)
{
    showToolTips = value;
}

void ConverterConfigurationEditDialog::promptForExecutableLocation() {
    QString s(tr("Please locate the execuatble file "));
    if (!getConverterDefinition().executable.isEmpty())
        s.append(getConverterDefinition().executable);

#if defined (Q_OS_WIN)
    QString filter = "*.exe";
#else
    QString filter = "";
#endif

    QString cp = QFileDialog::getOpenFileName(this, s, "",  filter);

    if(!cp.isNull()) {
        executablePathEdit->setText(QDir::toNativeSeparators(cp));
    }
}

bool ConverterConfigurationEditDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::ToolTip) // Intercept tooltip event
        return (showToolTips);

    else
        return ConverterConfigurationEditDialog::eventFilter(obj, event);
}
