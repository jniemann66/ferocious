#ifndef LPFPARAMETERSDLG_H
#define LPFPARAMETERSDLG_H

#include <QDialog>
#include <QPair>

namespace Ui {
class lpfParametersDlg;
}

class lpfParametersDlg : public QDialog
{
    Q_OBJECT

public:
    explicit lpfParametersDlg(QWidget *parent = 0);
    ~lpfParametersDlg();

    void setValues(double cutoff, double transition);
    QPair<double, double> getValues();
private:
    Ui::lpfParametersDlg *ui;
};

#endif // LPFPARAMETERSDLG_H
