#include "themeselectiondialog.h"

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
	auto mainLayout = new QVBoxLayout;
	auto stdButtons = new QDialogButtonBox(QDialogButtonBox::RestoreDefaults | QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	completer->setModel(themeModel);
	themeSelector->setCompleter(completer);

	mainLayout->addStretch();
	mainLayout->addWidget(themeSelector);
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

void ThemeSelectionDialog::setSelectedTheme(const QString& val)
{
	themeSelector->setText(val);
}


