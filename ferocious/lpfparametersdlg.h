/*
* Copyright (C) 2016 - 2026 Judd Niemann - All Rights Reserved.
* You may use, distribute and modify this code under the
* terms of the GNU Lesser General Public License, version 2.1
*
* You should have received a copy of GNU Lesser General Public License v2.1
* with this file. If not, please refer to: https://github.com/jniemann66/ferocious
*/

#ifndef LPFPARAMETERSDLG_H
#define LPFPARAMETERSDLG_H

// lpfParametersDlg - Dialog for entering LowPass Filter Parameters

#include <QDialog>
#include <QPair>

namespace Ui {
class LpfParametersDlg;
}

class LpfParametersDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LpfParametersDlg(QWidget *parent = nullptr);
    ~LpfParametersDlg() override;

    void setValues(double cutoff, double transition);
    void setNyquistFrequency(double Hz);
    QPair<double, double> getValues();

private:
    Ui::LpfParametersDlg *ui;
    double nyquistFrequency{0.0};

};

#endif // LPFPARAMETERSDLG_H
