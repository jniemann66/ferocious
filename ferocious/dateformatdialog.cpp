#include "dateformatdialog.h"

#include <QButtonGroup>
#include <QComboBox>
#include <QDateTime>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

static const QStringList commonFormats = {
    // ISO 8601 text variants — same structure as the Qt enum versions but editable
    "yyyy-MM-ddTHH:mm:ss",
    "yyyy-MM-ddTHH:mm:ss.zzz",
    "yyyy-MM-ddTHH:mm:ss t",
    "yyyy-MM-ddTHH:mm:ss.zzz t",
    // ISO 8601 with space separator (common in databases / logs)
    "yyyy-MM-dd HH:mm:ss",
    "yyyy-MM-dd HH:mm:ss.zzz",
    "yyyy-MM-dd HH:mm:ss t",
    "yyyy-MM-dd HH:mm:ss.zzz t",
    // European
    "dd/MM/yyyy HH:mm:ss",
    "dd/MM/yyyy HH:mm:ss.zzz",
    // US
    "MM/dd/yyyy HH:mm:ss",
    // Readable long forms
    "dd MMM yyyy HH:mm:ss",
    "dd MMM yyyy HH:mm:ss.zzz",
    "ddd dd MMM yyyy HH:mm:ss",
    "d MMMM yyyy HH:mm:ss",
    // Time only
    "HH:mm:ss",
    "HH:mm:ss.zzz",
    // Date only
    "yyyy-MM-dd",
    "dd/MM/yyyy",
    "dd MMM yyyy",
};

DateFormatDialog::DateFormatDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Timestamp Format"));

    radioTextDate      = new QRadioButton(tr("Text date (custom format)"));
    radioISODate       = new QRadioButton(tr("ISO 8601"));
    radioISODateWithMs = new QRadioButton(tr("ISO 8601 with milliseconds"));
    radioRFC2822       = new QRadioButton(tr("RFC 2822"));

    radioISODateWithMs->setChecked(true);

    textFormatCombo = new QComboBox;
    textFormatCombo->setEditable(true);
    textFormatCombo->addItems(commonFormats);
    textFormatCombo->setCurrentIndex(0);
    textFormatCombo->setEnabled(false);

    previewLabel = new QLabel;

    auto* formatGroup = new QGroupBox(tr("Date format"));
    auto* formatLayout = new QVBoxLayout;
    formatLayout->addWidget(radioTextDate);
    formatLayout->addWidget(textFormatCombo);
    formatLayout->addWidget(radioISODate);
    formatLayout->addWidget(radioISODateWithMs);
    formatLayout->addWidget(radioRFC2822);
    auto* exampleRow = new QHBoxLayout;
    exampleRow->addWidget(new QLabel(tr("Example:")), 1);
    exampleRow->addWidget(previewLabel, 5, Qt::AlignRight);
    formatLayout->addSpacing(4);
    formatLayout->addLayout(exampleRow);
    formatGroup->setLayout(formatLayout);

    showStartTimeCheckBox = new QCheckBox(tr("Show start time"));
    showStartTimeCheckBox->setChecked(true);
    showEndTimeCheckBox = new QCheckBox(tr("Show end time"));
    showEndTimeCheckBox->setChecked(true);

    separatorEdit = new QLineEdit(QStringLiteral(" -- "));
    separatorEdit->setMaximumWidth(120);

    auto* timestampGroup = new QGroupBox(tr("Timestamps"));
    auto* timestampLayout = new QFormLayout;
    timestampLayout->addRow(showStartTimeCheckBox);
    timestampLayout->addRow(showEndTimeCheckBox);
    timestampLayout->addRow(tr("Separator:"), separatorEdit);
    timestampGroup->setLayout(timestampLayout);

    auto* stdButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    auto* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(formatGroup);
    mainLayout->addWidget(timestampGroup);
    mainLayout->addStretch();
    mainLayout->addWidget(stdButtons);
    setLayout(mainLayout);

    auto* buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(radioTextDate);
    buttonGroup->addButton(radioISODate);
    buttonGroup->addButton(radioISODateWithMs);
    buttonGroup->addButton(radioRFC2822);

    connect(radioTextDate, &QRadioButton::toggled, textFormatCombo, &QComboBox::setEnabled);
    connect(buttonGroup, &QButtonGroup::buttonClicked, this, &DateFormatDialog::updatePreview);
    connect(textFormatCombo, &QComboBox::currentTextChanged, this, &DateFormatDialog::updatePreview);
    connect(stdButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(stdButtons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    updatePreview();
}

std::variant<Qt::DateFormat, QString> DateFormatDialog::getDateFormat() const
{
    if (radioTextDate->isChecked()) {
        const QString text = textFormatCombo->currentText().trimmed();
        if (!text.isEmpty())
            return text;
        return Qt::TextDate;
    }
    if (radioISODate->isChecked())
        return Qt::ISODate;
    if (radioRFC2822->isChecked())
        return Qt::RFC2822Date;
    return Qt::ISODateWithMs;
}

void DateFormatDialog::setDateFormat(const std::variant<Qt::DateFormat, QString>& fmt)
{
    if (std::holds_alternative<QString>(fmt)) {
        radioTextDate->setChecked(true);
        textFormatCombo->setCurrentText(std::get<QString>(fmt));
        return;
    }

    switch (std::get<Qt::DateFormat>(fmt)) {
    case Qt::TextDate:
        radioTextDate->setChecked(true);
        break;
    case Qt::ISODate:
        radioISODate->setChecked(true);
        break;
    case Qt::RFC2822Date:
        radioRFC2822->setChecked(true);
        break;
    case Qt::ISODateWithMs:
    default:
        radioISODateWithMs->setChecked(true);
        break;
    }
}

bool DateFormatDialog::getShowStartTime() const
{
    return showStartTimeCheckBox->isChecked();
}

void DateFormatDialog::setShowStartTime(bool show)
{
    showStartTimeCheckBox->setChecked(show);
}

bool DateFormatDialog::getShowEndTime() const
{
    return showEndTimeCheckBox->isChecked();
}

void DateFormatDialog::setShowEndTime(bool show)
{
    showEndTimeCheckBox->setChecked(show);
}

QString DateFormatDialog::getTimestampSeparator() const
{
    return separatorEdit->text();
}

void DateFormatDialog::setTimestampSeparator(const QString& sep)
{
    separatorEdit->setText(sep);
}

void DateFormatDialog::updatePreview()
{
    const QDateTime now = QDateTime::currentDateTime();
    QString preview;

    if (radioTextDate->isChecked()) {
        const QString fmt = textFormatCombo->currentText().trimmed();
        preview = fmt.isEmpty() ? now.toString(Qt::TextDate) : now.toString(fmt);
    } else if (radioISODate->isChecked()) {
        preview = now.toString(Qt::ISODate);
    } else if (radioRFC2822->isChecked()) {
        preview = now.toString(Qt::RFC2822Date);
    } else {
        preview = now.toString(Qt::ISODateWithMs);
    }

    previewLabel->setText(preview);
}
