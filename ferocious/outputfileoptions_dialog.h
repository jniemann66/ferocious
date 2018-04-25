#ifndef OUTPUTFILEOPTIONS_DIALOG_H
#define OUTPUTFILEOPTIONS_DIALOG_H

// outputfileoptionsdialog.h
// by J. Niemann
// defines FilenameGenerator class and
// OutputFileOptions_Dialog class
//

#include <QDialog>
#include <QSettings>
#include <QDir>
#include <qdebug.h>

// class FilenameGenerator. Purpose:
// 1. Keep a persistent record of user preferences for naming of output file
// 2. Generate output filename, given an input filename, and by applying preferences
// 3. Provide Loading and Saving services (for persistence), given a QSettings object.

class FilenameGenerator {
public:
    FilenameGenerator();
    FilenameGenerator(const FilenameGenerator& O);
    void generateOutputFilename(QString& outFilename, const QString& inFilename, const QString subDirectory = QString());
    void saveSettings(QSettings& settings);
    void loadSettings(QSettings& settings);

    bool appendSuffix;
    bool useSpecificOutputDirectory;
    bool replicateDirectoryStructure;
    bool useSpecificFileExt;

    QString Suffix;
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
    explicit OutputFileOptions_Dialog(FilenameGenerator& OFN, QWidget *parent = nullptr);
    ~OutputFileOptions_Dialog();

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
