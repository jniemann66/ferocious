#include "lpfparametersdlg.h"
#include "ui_lpfparametersdlg.h"


lpfParametersDlg::lpfParametersDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::lpfParametersDlg)
{
    ui->setupUi(this);
}

void lpfParametersDlg::setValues(double cutoff, double transition) {
    ui->cutoffSpinBox->setValue(cutoff);
    ui->transitionSpinBox->setValue(transition);
}

QPair<double, double> lpfParametersDlg::getValues() {
    QPair<double, double> v;
    v.first = ui->cutoffSpinBox->value();
    v.second = ui->transitionSpinBox->value();
    return v;
}

lpfParametersDlg::~lpfParametersDlg()
{
    delete ui;
}
