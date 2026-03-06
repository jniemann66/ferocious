#include "themeselectiondialog.h"

#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDialogButtonBox>

const QString ThemeSelectionDialog::defaultTheme{"ferocious"};

ThemeSelectionDialog::ThemeSelectionDialog(QWidget *parent, Qt::WindowFlags f)
	: QDialog(parent, f)
{
	themeSelector = new QLineEdit;
	completer = new QCompleter;
	themeModel = new QStringListModel(
				QStringList{
					"amber-fluid",
					"ferocious-blue",
					"ferocious",
					"flat",
					"native",
					"square"
				});
	useNativeDialogsCheckBox = new QCheckBox(tr("Use Native Dialogs"));
	useNativeDialogsCheckBox->setToolTip(tr("Use the operating system's built-in file dialogs.\n"
											"Uncheck to use Qt's built-in dialogs, which support\n"
											"audio preview when browsing for input files."));

	auto mainLayout = new QVBoxLayout;
	auto stdButtons = new QDialogButtonBox(QDialogButtonBox::RestoreDefaults | QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	completer->setModel(themeModel);
	themeSelector->setCompleter(completer);

	mainLayout->addStretch();
	mainLayout->addWidget(themeSelector);
	mainLayout->addWidget(useNativeDialogsCheckBox);
	mainLayout->addStretch();
	mainLayout->addWidget(stdButtons);
	setLayout(mainLayout);

	completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
	themeSelector->setText(ThemeSelectionDialog::defaultTheme);

	auto restoreBtn = stdButtons->button(QDialogButtonBox::RestoreDefaults);
	connect (restoreBtn, &QPushButton::clicked, this, [this]{
		themeSelector->setText(ThemeSelectionDialog::defaultTheme);
	});

	connect(stdButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(stdButtons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QString ThemeSelectionDialog::getSelectedTheme() const
{
	return themeSelector->text();
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
	themeSelector->setText(val);
}

void ThemeSelectionDialog::setSelectedThemeFilename(const QString& val)
{
	static const QRegularExpression rx(R"(:\/Themes\/(.*).css)");
	auto rxm = rx.match(val);
	if (rxm.hasMatch()) {
		themeSelector->setText(rxm.captured(1));
	}
}
