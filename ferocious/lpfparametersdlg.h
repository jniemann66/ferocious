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
    ~LpfParametersDlg();

    void setValues(double cutoff, double transition);
    void setNyquistFrequency(double Hz);
    QPair<double, double> getValues();

private:
    Ui::LpfParametersDlg *ui;
    double nyquistFrequency;

};

#endif // LPFPARAMETERSDLG_H
