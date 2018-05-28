#ifndef CONVERTERCONFIGURATIONDIALOG_H
#define CONVERTERCONFIGURATIONDIALOG_H

#include "convertersmodel.h"

#include <QObject>
#include <QDialog>
#include <QTableView>


class ConverterConfigurationDialog : public QDialog
{
public:
    ConverterConfigurationDialog(QWidget *parent);

private:
    QTableView tableView;
    ConvertersModel convertersModel;
};

#endif // CONVERTERCONFIGURATIONDIALOG_H
