/*
* Copyright (C) 2016 - 2021 Judd Niemann - All Rights Reserved.
* You may use, distribute and modify this code under the
* terms of the GNU Lesser General Public License, version 2.1
*
* You should have received a copy of GNU Lesser General Public License v2.1
* with this file. If not, please refer to: https://github.com/jniemann66/ferocious
*/

#ifndef OUTPUTFILEOPTIONS_DIALOG_H
#define OUTPUTFILEOPTIONS_DIALOG_H

// outputfileoptionsdialog.h
// defines FilenameGenerator class and
// OutputFileOptions_Dialog class

#include <QDialog>
#include <QSettings>
#include <QDir>
#include <QDebug>

// class FilenameGenerator. Purpose:
// 1. Keep a persistent record of user preferences for naming of output file
// 2. Generate output filename, given an input filename, and by applying preferences
// 3. Provide Loading and Saving services (for persistence), given a QSettings object.

class FilenameGenerator {
public:
    FilenameGenerator();
	FilenameGenerator(const FilenameGenerator& o) = default;
	void generateOutputFilename(QString& outFilename, const QString& inFilename, const QString& subDirectory = {});
    void saveSettings(QSettings& settings);
    void loadSettings(QSettings& settings);

    bool appendSuffix;
    bool useSpecificOutputDirectory;
    bool replicateDirectoryStructure;
    bool useSpecificFileExt;

    QString suffix;
    QString inputDirectoryRoot;
    QString outputDirectory;
    QString fileExt;
};

namespace Ui {
    class OutputFileOptions_Dialog;
}

class OutputFileOptions_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit OutputFileOptions_Dialog(FilenameGenerator& filenameGenerator, QWidget *parent = nullptr);
    ~OutputFileOptions_Dialog() override;

private slots:
    void on_FilenameSuffix_checkBox_clicked();
    void on_useOutputDirectory_checkBox_clicked();
    void on_setFileExt_radioButton_clicked();
    void on_SameFileExt_radioButton_clicked();
    void on_OutputFileOptions_buttonBox_accepted();
    void on_pushButton_clicked();

private:
    Ui::OutputFileOptions_Dialog *ui;
    FilenameGenerator* pFilenameGenerator;
};

#endif // OUTPUTFILEOPTIONS_DIALOG_H
