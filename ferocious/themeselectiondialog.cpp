#include "themeselectiondialog.h"

#include <QCheckBox>
#include <QDir>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDialogButtonBox>

const QString ThemeSelectionDialog::defaultTheme{"ferocious"};

ThemeSelectionDialog::ThemeSelectionDialog(QWidget *parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
	themeSelector = new QComboBox;
	for (const QFileInfo &fi : QDir(":/Themes").entryInfoList({"*.css"}, QDir::Files, QDir::Name)) {
		themeSelector->addItem(fi.baseName());
	}
	useNativeDialogsCheckBox = new QCheckBox(tr("Use Native Dialogs"));
	useNativeDialogsCheckBox->setToolTip(tr("Use the operating system's built-in file dialogs.\n"
											"Uncheck to use Qt's built-in dialogs, which support\n"
											"audio preview when browsing for input files."));

	auto mainLayout = new QVBoxLayout;
	auto stdButtons = new QDialogButtonBox(QDialogButtonBox::RestoreDefaults | QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	mainLayout->addStretch();
	mainLayout->addWidget(themeSelector);
	mainLayout->addWidget(useNativeDialogsCheckBox);
	mainLayout->addStretch();
	mainLayout->addWidget(stdButtons);
	setLayout(mainLayout);

	themeSelector->setCurrentText(ThemeSelectionDialog::defaultTheme);

	auto restoreBtn = stdButtons->button(QDialogButtonBox::RestoreDefaults);
	connect(restoreBtn, &QPushButton::clicked, this, [this]{
		themeSelector->setCurrentText(ThemeSelectionDialog::defaultTheme);
	});

	connect(stdButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(stdButtons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QString ThemeSelectionDialog::getSelectedTheme() const
{
	return themeSelector->currentText();
}

QString ThemeSelectionDialog::getSelectedThemeFilename() const
{
	return QStringLiteral(":/Themes/%1.css").arg(getSelectedTheme());
}

bool ThemeSelectionDialog::getUseNativeDialogs() const
{
	return useNativeDialogsCheckBox->isChecked();
}

void ThemeSelectionDialog::setUseNativeDialogs(bool val)
{
	useNativeDialogsCheckBox->setChecked(val);
}

void ThemeSelectionDialog::setSelectedTheme(const QString& val)
{
	themeSelector->setCurrentText(val);
}

void ThemeSelectionDialog::setSelectedThemeFilename(const QString& val)
{
	static const QRegularExpression rx(R"(:\/Themes\/(.*).css)");
	auto rxm = rx.match(val);
	if (rxm.hasMatch()) {
		themeSelector->setCurrentText(rxm.captured(1));
	}
}
