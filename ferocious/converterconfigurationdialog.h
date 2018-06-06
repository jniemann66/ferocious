#ifndef CONVERTERCONFIGURATIONDIALOG_H
#define CONVERTERCONFIGURATIONDIALOG_H

#include "convertersmodel.h"
#include "fancylineedit.h"
#include "converterconfigurationeditdialog.h"

#include <QObject>
#include <QDialog>
#include <QLineEdit>
#include <QTableView>
#include <Qmenu>

class ConverterConfigurationDialog : public QDialog
{
    Q_OBJECT

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
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onEditRequested(const QModelIndex &modelIndex);
    void onDeleteRequested(const QModelIndex &modelIndex);
    void onCloneRequested(const QModelIndex &modelIndex);

private:
    QLabel* mainConverterLocationLabel;
    QLabel* additionalConvertersLabel;
    FancyLineEdit* mainConverterLocationEdit;
    QPushButton* browseButton;
    QMenu* contextMenu;
    QTableView tableView;
    ConvertersModel convertersModel;
    QString expectedMainConverter;
    QString mainConverterPath;


    void promptForResamplerLocation();
};

#endif // CONVERTERCONFIGURATIONDIALOG_H
