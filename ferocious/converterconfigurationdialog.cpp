#include "converterconfigurationdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QDialogButtonBox>

ConverterConfigurationDialog::ConverterConfigurationDialog(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    // allocate
    auto mainLayout = new QVBoxLayout;
    auto mainConverterLayout = new QHBoxLayout;
    mainConverterLocationLabel = new QLabel("Location of Main Converter:");
    mainConverterLocationEdit = new FancyLineEdit;
    browseButton = new QPushButton("Browse ...");

    additionalConvertersLabel = new QLabel("Additional converters:");
    auto stdButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    // set model
    tableView.setModel(&convertersModel);

    // configure widgets
    mainConverterLocationEdit->hideEditButton();

    // attach widgets to main layout
    mainLayout->addWidget(mainConverterLocationLabel);
    mainConverterLayout->addWidget(mainConverterLocationEdit);
    mainConverterLayout->addWidget(browseButton);
    mainLayout->addLayout(mainConverterLayout);

    mainLayout->addWidget(additionalConvertersLabel);
    mainLayout->addWidget(&tableView);
    mainLayout->addWidget(stdButtons);
    setLayout(mainLayout);

    // connect signals / slots
    connect(mainConverterLocationEdit, &QLineEdit::editingFinished, this, [this]{
       mainConverterPath = mainConverterLocationEdit->text();
    });
    connect(browseButton, &QPushButton::clicked, this, &ConverterConfigurationDialog::promptForResamplerLocation);
    connect(stdButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(stdButtons, &QDialogButtonBox::rejected, this, &QDialog::reject);



}

void ConverterConfigurationDialog::showEvent(QShowEvent* event) {

    if(mainConverterPath.isEmpty() || !QFile::exists(mainConverterPath)) {
       promptForResamplerLocation();
    }

    mainConverterLocationLabel->setText(QString{"Location of Main Converter (%1):"}.arg(expectedMainConverter));
    mainConverterLocationEdit->setText(mainConverterPath);

    QDialog::showEvent(event);
}

QVector<ConverterDefinition> ConverterConfigurationDialog::getConverterDefinitions() const
{
    return convertersModel.getConverterDefinitions();
}

void ConverterConfigurationDialog::setConverterDefinitions(const QVector<ConverterDefinition> &value)
{
    convertersModel.setConverterDefinitions(value);
}


void ConverterConfigurationDialog::promptForResamplerLocation() {
    QString s("Please locate the file: ");
    s.append(expectedMainConverter);

#if defined (Q_OS_WIN)
    QString filter = "*.exe";
#else
    QString filter = "";
#endif

    QString cp = QFileDialog::getOpenFileName(this, s, mainConverterPath,  filter);

    if(!cp.isNull()) {
        mainConverterPath = cp;
        if(mainConverterPath.lastIndexOf(expectedMainConverter, -1, Qt::CaseInsensitive) == -1) { // safeguard against wrong executable being configured
            mainConverterPath.clear();
            QMessageBox::warning(this, tr("Converter Location"), tr("That is not the right program!\n"), QMessageBox::Ok);
        }
    }
}

QString ConverterConfigurationDialog::getMainConverterPath() const
{
    return mainConverterPath;
}

void ConverterConfigurationDialog::setMainConverterPath(const QString &value)
{
    mainConverterPath = value;
}

QString ConverterConfigurationDialog::getExpectedMainConverter() const
{
    return expectedMainConverter;
}

void ConverterConfigurationDialog::setExpectedMainConverter(const QString &value)
{
    expectedMainConverter = value;
}
