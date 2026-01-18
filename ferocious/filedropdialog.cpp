/*
* Copyright (C) 2016 - 2026 Judd Niemann - All Rights Reserved.
* You may use, distribute and modify this code under the
* terms of the GNU Lesser General Public License, version 2.1
*
* You should have received a copy of GNU Lesser General Public License v2.1
* with this file. If not, please refer to: https://github.com/jniemann66/ferocious
*/

#include "filedropdialog.h"

FileDropDialog::FileDropDialog(QWidget *parent)
    : QDialog(parent)
{
    // Allocate widgets
    textLabel = new QLabel;
    textEdit = new FileDropTextEdit(this);
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    // attach to layout
    auto* mainLayout = new QVBoxLayout;
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
