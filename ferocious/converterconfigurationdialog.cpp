#include "converterconfigurationdialog.h"


#include <QVBoxLayout>


ConverterConfigurationDialog::ConverterConfigurationDialog(QWidget* parent) : QDialog(parent)
{
    auto mainLayout = new QVBoxLayout;

    tableView.setModel(&convertersModel);
    mainLayout->addWidget(&tableView);
    setLayout(mainLayout);
}


