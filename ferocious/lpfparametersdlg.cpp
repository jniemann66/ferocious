#include "lpfparametersdlg.h"
#include "ui_lpfparametersdlg.h"

lpfParametersDlg::lpfParametersDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::lpfParametersDlg),
    nyquistFrequency(0.0)
{
    ui->setupUi(this);

    connect(ui->cutoffSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [this](double val) {
       if(nyquistFrequency != 0.0) {
           QString suffix = " %   (" + QString::number(nyquistFrequency * 0.01 * val, 'f', 2) + " Hz)";
           ui->cutoffSpinBox->setSuffix(suffix);
       }
    });

    connect(ui->transitionSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, [this](double val) {
       if(nyquistFrequency != 0.0) {
           QString suffix = " %   (" + QString::number(nyquistFrequency * 0.01 * val, 'f', 2) + " Hz)";
           ui->transitionSpinBox->setSuffix(suffix);
       }
    });
}

void lpfParametersDlg::setValues(double cutoff, double transition) {
    ui->cutoffSpinBox->setValue(cutoff);
    ui->transitionSpinBox->setValue(transition);
}

void lpfParametersDlg::setNyquistFrequency(double Hz) {
    nyquistFrequency = Hz;
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
