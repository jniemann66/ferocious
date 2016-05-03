#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

#include "outputfileoptions_dialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QString ConverterPath;

private slots:
    void on_StdoutAvailable();
    void on_ConverterStarted();
    void on_ConverterFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void on_browseInfileButton_clicked();
    void on_convertButton_clicked();
    void on_InfileEdit_editingFinished();
    void on_browseOutfileButton_clicked();
    void on_NormalizeCheckBox_clicked();
    void on_NormalizeAmountEdit_editingFinished();
    void on_BitDepthCheckBox_clicked();
    void on_OutfileEdit_editingFinished();

    void on_DitherCheckBox_clicked();

    void on_DitherAmountEdit_editingFinished();

    void on_actionConverter_Location_triggered();

    void on_actionOutput_File_Options_triggered();

    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
    QProcess Converter;

    void PopulateBitFormats(const QString& fileName);   // poulate combobox with list of subformats returned from query to converter
    bool fileExists(const QString& path);   // detect if file represented by path exists
    void writeSettings();       // write settings to ini file
    void readSettings();        // read settings from ini file
    QString lastOutputFileExt;  // used for tracking if user changed the file extension when changing the output filename
    QString inFileBrowsePath;   // used for storing the the path on "open input file" Dialog
    QString outFileBrowsePath;  // used for storing the the path on "open output file" Dialog
    OutFileNamer outfileNamer;
    QString ResamplerVersion;
    void getResamplerVersion(QString &v);
    void ProcessOutfileExtension();
};



#endif // MAINWINDOW_H
