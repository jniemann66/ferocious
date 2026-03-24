#include "concurrentconversionsdialog.h"

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <thread>

ConcurrentConversionsDialog::ConcurrentConversionsDialog(int current, int recommended, int maxValue, QWidget* parent)
    : QDialog(parent), recommended(recommended)
{
    setWindowTitle(tr("Concurrent Conversions"));

    slider = new QSlider(Qt::Horizontal);
    slider->setRange(1, maxValue);
    slider->setValue(current);
    slider->setTickPosition(QSlider::TicksBelow);
    slider->setTickInterval(1);

    lineEdit = new QLineEdit;
    lineEdit->setValidator(new QIntValidator(1, maxValue, this));
    lineEdit->setFixedWidth(45);
    lineEdit->setAlignment(Qt::AlignRight);
    lineEdit->setText(QString::number(current));

    const int hwThreads = static_cast<int>(std::thread::hardware_concurrency());
    auto* hintLabel = new QLabel(tr("Recommended: %1  (detected %2 hardware threads)")
                                     .arg(recommended)
                                     .arg(hwThreads));
    hintLabel->setAlignment(Qt::AlignCenter);

    auto* sliderRow = new QHBoxLayout;
    sliderRow->addWidget(slider);
    sliderRow->addWidget(lineEdit);

    auto* stdButtons = new QDialogButtonBox(
        QDialogButtonBox::RestoreDefaults | QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    auto* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(new QLabel(tr("Number of simultaneous conversions:")));
    mainLayout->addLayout(sliderRow);
    mainLayout->addWidget(hintLabel);
    mainLayout->addStretch();
    mainLayout->addWidget(stdButtons);
    setLayout(mainLayout);

    connect(slider, &QSlider::valueChanged, this, [this](int v) {
        lineEdit->setText(QString::number(v));
    });

    connect(lineEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
        bool ok{false};
        const int v = text.toInt(&ok);
        if (ok && v >= slider->minimum() && v <= slider->maximum())
            slider->setValue(v);
    });

    connect(stdButtons->button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked,
            this, [this]() {
                slider->setValue(this->recommended);
            });

    connect(stdButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(stdButtons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

int ConcurrentConversionsDialog::getValue() const
{
    return slider->value();
}
