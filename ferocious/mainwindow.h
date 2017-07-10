#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

#if defined (Q_OS_WIN)
const char expectedConverter[] ="resampler.exe";
#elif defined (Q_OS_LINUX)
const char expectedConverter[] ="resampler";
#else
const char expectedConverter[] ="ReSampler";
#endif

#include "outputfileoptions_dialog.h"

class conversionTask{
public:
    QString inFilename;
    QString outFilename;
};

namespace Ui {
class MainWindow;
}

typedef enum {
    noiseShape_standard,
    noiseShape_flatTpdf
} NoiseShape;

typedef enum {
    relaxedLPF = 0,
    standardLPF,
    steepLPF,
    customLPF
} LPFType;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString ConverterPath;
    QString MultiFileSeparator;

protected:
    bool eventFilter(QObject *obj, QEvent *event);

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
    void on_actionAbout_Qt_triggered();
    void on_actionFlac_triggered();
    void on_actionOgg_Vorbis_triggered();
    void on_actionEnable_Clipping_Protection_triggered();
    void on_actionTheme_triggered();
    void on_actionRelaxedLPF_triggered();
    void on_actionStandardLPF_triggered();
    void on_actionSteepLPF_triggered();
    void on_actionFixed_Seed_triggered();
    void on_actionSeed_Value_triggered();
    void on_actionNoiseShapingStandard_triggered();
    void on_actionNoiseShapingFlatTpdf_triggered();
    void on_actionEnable_Multi_Threading_triggered(bool checked);

    void on_actionCustomLPF_triggered();

    void on_actionCustom_Parameters_triggered();

private:
    Ui::MainWindow *ui;
    QProcess Converter;
    QVector<conversionTask> conversionQueue;
    QString lastOutputFileExt;  // used for tracking if user changed the file extension when changing the output filename
    QString inFileBrowsePath;   // used for storing the the path on "open input file" Dialog
    QString outFileBrowsePath;  // used for storing the the path on "open output file" Dialog
    QString stylesheetFilePath;
    FilenameGenerator filenameGenerator;  // output filename generator
    QString ResamplerVersion;   // version string of main external converter
    bool bShowProgressBar;
    int flacCompressionLevel;
    double vorbisQualityLevel;
    LPFType LPFtype;
    double customLpfCutoff;
    double customLpfTransition;
    bool bDisableClippingProtection; // if true, the --noClippingProtection switch will be sent to the converter
    bool bEnableMultithreading; // if true, issue --mt option
    bool bFixedSeed;
    int seedValue;
    NoiseShape noiseShape;
    int ditherProfile;
    void PopulateBitFormats(const QString& fileName);   // poulate combobox with list of subformats returned from query to converter
    bool fileExists(const QString& path);   // detect if file represented by path exists
    void writeSettings();       // write settings to ini file
    void readSettings();        // read settings from ini file
    void getResamplerVersion();   // function to retrieve version number of main external converter
    void ProcessOutfileExtension(); // function to update combobox etc when a new output file extension is chosen
    void convert(const QString &outfn, const QString &infn); // execute a conversion task
    void wildcardPushToQueue(const QString &inFilename); // interpret filename containing wildcards, and push tasks onto queue as appropraite
    void convertNext(); // perform conversion task from front of queue, then remove task from queue
    void applyStylesheet();
    void populateDitherProfileMenu();
    void clearNoiseShapingMenu();
    void on_action_DitherProfile_triggered(QAction* action, int id); // not using old "private slots:" system (just an ordinary member function.)
    void getCustomLpfParameters();
};

#endif // MAINWINDOW_H
