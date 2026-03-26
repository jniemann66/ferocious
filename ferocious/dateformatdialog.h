#ifndef DATEFORMATDIALOG_H
#define DATEFORMATDIALOG_H

#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <variant>

class DateFormatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DateFormatDialog(QWidget* parent = nullptr);

    std::variant<Qt::DateFormat, QString> getDateFormat() const;
    void setDateFormat(const std::variant<Qt::DateFormat, QString>& fmt);

    bool getShowStartTime() const;
    void setShowStartTime(bool show);

    bool getShowEndTime() const;
    void setShowEndTime(bool show);

    QString getTimestampSeparator() const;
    void setTimestampSeparator(const QString& sep);

private:
    QRadioButton* radioTextDate{nullptr};
    QRadioButton* radioISODate{nullptr};
    QRadioButton* radioISODateWithMs{nullptr};
    QRadioButton* radioRFC2822{nullptr};
    QComboBox* textFormatCombo{nullptr};
    QCheckBox* showStartTimeCheckBox{nullptr};
    QCheckBox* showEndTimeCheckBox{nullptr};
    QLineEdit* separatorEdit{nullptr};
    QLabel* previewLabel{nullptr};

private slots:
    void updatePreview();
};

#endif // DATEFORMATDIALOG_H
