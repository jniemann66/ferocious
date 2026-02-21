#ifndef THEMESELECTIONDIALOG_H
#define THEMESELECTIONDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QCompleter>
#include <QStringListModel>
#include <QLineEdit>

class ThemeSelectionDialog : public QDialog
{
	Q_OBJECT

public:
	ThemeSelectionDialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

	QString getSelectedTheme() const;
	QString getSelectedThemeFilename() const;

	void setSelectedTheme(const QString& val);

private:
	QLineEdit *themeSelector{nullptr};
	QStringListModel *themeModel{nullptr};
	QCompleter *completer{nullptr};

	static const QString defaultTheme;
};

#endif // THEMESELECTIONDIALOG_H
