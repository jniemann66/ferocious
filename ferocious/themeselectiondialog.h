#ifndef THEMESELECTIONDIALOG_H
#define THEMESELECTIONDIALOG_H

#include <QCheckBox>
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
	bool getUseNativeDialogs() const;

	void setSelectedTheme(const QString& val);
	void setSelectedThemeFilename(const QString &val);
	void setUseNativeDialogs(bool val);

private:
	QLineEdit *themeSelector{nullptr};
	QStringListModel *themeModel{nullptr};
	QCompleter *completer{nullptr};
	QCheckBox *useNativeDialogsCheckBox{nullptr};

	static const QString defaultTheme;
};

#endif // THEMESELECTIONDIALOG_H
