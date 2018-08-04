#include "filedropdialog.h"

FileDropDialog::FileDropDialog(QWidget *parent) : QDialog(parent)
{
    // Allocate widgets
    textLabel = new QLabel;
    textEdit = new FileDropTextEdit(this);
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    // attach to layout
    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(textLabel);
    mainLayout->addWidget(textEdit);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    // connect signals / slots
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void FileDropDialog::setText(const QString &title, const QString &label, const QString &text)
{
    setWindowTitle(title);
    textLabel->setText(label);
    textEdit->setPlainText(text);
}

QString FileDropDialog::getText() const
{
    return textEdit->toPlainText();
}
