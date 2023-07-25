/*
* Copyright (C) 2016 - 2023 Judd Niemann - All Rights Reserved.
* You may use, distribute and modify this code under the
* terms of the GNU Lesser General Public License, version 2.1
*
* You should have received a copy of GNU Lesser General Public License v2.1
* with this file. If not, please refer to: https://github.com/jniemann66/ferocious
*/

#include "lpfparametersdlg.h"
#include "ui_lpfparametersdlg.h"

LpfParametersDlg::LpfParametersDlg(QWidget *parent) : QDialog(parent), ui(new Ui::LpfParametersDlg)
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

void LpfParametersDlg::setValues(double cutoff, double transition) {
    ui->cutoffSpinBox->setValue(cutoff);
    ui->transitionSpinBox->setValue(transition);
}

void LpfParametersDlg::setNyquistFrequency(double Hz) {
    nyquistFrequency = Hz;
}

QPair<double, double> LpfParametersDlg::getValues() {
    QPair<double, double> v;
    v.first = ui->cutoffSpinBox->value();
    v.second = ui->transitionSpinBox->value();
    return v;
}

LpfParametersDlg::~LpfParametersDlg()
{
    delete ui;
}
