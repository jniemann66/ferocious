/*
* Copyright (C) 2016 - 2026 Judd Niemann - All Rights Reserved.
* You may use, distribute and modify this code under the
* terms of the GNU Lesser General Public License, version 2.1
*
* You should have received a copy of GNU Lesser General Public License v2.1
* with this file. If not, please refer to: https://github.com/jniemann66/ferocious
*/

#ifndef CONVERTERCONFIGURATIONEDITDIALOG_H
#define CONVERTERCONFIGURATIONEDITDIALOG_H

#include "converterdefinition.h"
#include "fancylineedit.h"

#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QPushButton>

class ConverterConfigurationEditDialog : public QDialog
{
public:
    ConverterConfigurationEditDialog(QWidget* parent);
    ConverterDefinition getConverterDefinition() const;
    void setConverterDefinition(const ConverterDefinition &converterDefinition);
    void setShowToolTips(bool value);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QLineEdit* priorityEdit;
    QCheckBox* enabledCheckbox;
    QLineEdit* nameEdit;
    QTextEdit* commentEdit;
    QLineEdit* downloadLocationEdit;
    QPushButton* openURLButton;
    QLineEdit* inputFileExtEdit;
    QLineEdit* outputFileExtEdit;
    QLineEdit* executableEdit;
    QLabel* executablePathLabel;
    FancyLineEdit* executablePathEdit;
    QPushButton* executablePathBrowseButton;
    QLineEdit* commandLineEdit;
    QDialogButtonBox* dialogButtonBox;

    bool showToolTips;

    void promptForExecutableLocation();
};

#endif // CONVERTERCONFIGURATIONEDITDIALOG_H

