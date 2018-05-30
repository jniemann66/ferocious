#ifndef CONVERTERCONFIGURATIONDIALOG_H
#define CONVERTERCONFIGURATIONDIALOG_H

#include "convertersmodel.h"
#include "fancylineedit.h"

#include <QObject>
#include <QDialog>
#include <QLineEdit>
#include <QTableView>


class ConverterConfigurationDialog : public QDialog
{
public:
    ConverterConfigurationDialog(QWidget *parent, Qt::WindowFlags f);

    QString getExpectedMainConverter() const;
    void setExpectedMainConverter(const QString &value);

    QString getMainConverterPath() const;
    void setMainConverterPath(const QString &value);

    QVector<ConverterDefinition> getConverterDefinitions() const;
    void setConverterDefinitions(const QVector<ConverterDefinition> &value);

public slots:

protected:
    void showEvent(QShowEvent* event) override;

private:
    QLabel* mainConverterLocationLabel;
    QLabel* additionalConvertersLabel;
    FancyLineEdit* mainConverterLocationEdit;
    QPushButton* browseButton;
    QTableView tableView;
    ConvertersModel convertersModel;
    QString expectedMainConverter;
    QString mainConverterPath;

    void promptForResamplerLocation();
};

#endif // CONVERTERCONFIGURATIONDIALOG_H
