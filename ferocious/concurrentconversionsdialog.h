#ifndef CONCURRENTCONVERSIONSDIALOG_H
#define CONCURRENTCONVERSIONSDIALOG_H

#include <QDialog>
#include <QSlider>
#include <QLineEdit>

class ConcurrentConversionsDialog : public QDialog
{
    Q_OBJECT

public:
    ConcurrentConversionsDialog(int current, int recommended, int maxValue, QWidget* parent = nullptr);
    int getValue() const;

private:
    QSlider* slider{nullptr};
    QLineEdit* lineEdit{nullptr};
    int recommended{1};
};

#endif // CONCURRENTCONVERSIONSDIALOG_H
