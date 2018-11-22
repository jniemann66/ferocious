#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "converterconfigurationdialog.h"

#include <QMainWindow>
#include <QProcess>
#include <QMenu>

#if defined (Q_OS_WIN)
const char expectedConverter[] ="resampler.exe";
#elif defined (Q_OS_LINUX)
const char expectedConverter[] ="resampler";
#else
const char expectedConverter[] ="ReSampler";
#endif

#include "outputfileoptions_dialog.h"
#include "converterdefinition.h"

const QString consoleGreen{"#66A334"};
const QString consoleYellow{"#D6C878"};
const QString consoleAmber{"#D6953E"};
const QString consoleRed{"#ff8080"};

class ConversionTask
{
public:
    QString inFilename;
    QString outFilename;
};

namespace Ui {
class MainWindow;
}

enum NoiseShape {
    noiseShape_standard,
    noiseShape_flatTpdf
};

enum LPFType {
    relaxedLPF = 0,
    standardLPF,
    steepLPF,
    customLPF
};

enum class LaunchType {
    Convert,
    Clipboard
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString converterPath;
    QString multiFileSeparator;
    static QStringList getQuotedArgs(const QStringList &args);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void on_StdoutAvailable();
    void on_StderrAvailable();
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
    void on_actionMultiStageConversion_triggered(bool checked);
    void on_actionUse_a_temp_file_triggered(bool checked);
    void on_actionCustomLPF_triggered();
    void on_actionCustom_Parameters_triggered();
    void on_convertButton_rightClicked();
    void on_stopRequested();
    void on_browseInButton_rightClicked();

private:
    // resources
    Ui::MainWindow *ui;
    QMenu* convertTaskMenu;
    QMenu* browseInMenu;
    ConverterConfigurationDialog* converterConfigurationDialog;

    // state
    QProcess process;
    QVector<ConversionTask> conversionQueue;
    QString lastOutputFileExt;  // used for tracking if user changed the file extension when changing the output filename
    QString inFileBrowsePath;   // used for storing the the path on "open input file" Dialog
    QString outFileBrowsePath;  // used for storing the the path on "open output file" Dialog
    QString stylesheetFilePath;
    FilenameGenerator filenameGenerator;  // output filename generator
    QString resamplerVersion;   // version string of main external converter
    bool bShowProgressBar;
    int flacCompressionLevel;
    double vorbisQualityLevel;
    LPFType lpfType;
    double customLpfCutoff;
    double customLpfTransition;
    bool bDisableClippingProtection; // if true, the --noClippingProtection switch will be sent to the converter
    bool bEnableMultithreading; // if true, issue --mt option
    bool bSingleStage;
    bool bNoTempFile;
    bool bFixedSeed;
    int seedValue;
    NoiseShape noiseShape;
    int ditherProfile;
    LaunchType launchType;
    QVector<ConverterDefinition> converterDefinitions;

    // functions
    void populateBitFormats(const QString& fileName);   // poulate combobox with list of subformats returned from query to converter
    bool fileExists(const QString& path);   // detect if file represented by path exists
    void writeSettings();       // write settings to ini file
    void readSettings();        // read settings from ini file
    void getResamplerVersion();   // function to retrieve version number of main external converter
    void processOutfileExtension(); // function to update combobox etc when a new output file extension is chosen
    void convert(const QString &outfn, const QString &infn); // execute a conversion task
    QStringList prepareMidConverterArgs(const QString &outfn, const QString &infn); // prepare commandline args for ReSampler
    void wildcardPushToQueue(const QString &inFilename); // interpret filename containing wildcards, and push tasks onto queue as appropraite
    void convertNext(); // perform conversion task from front of queue, then remove task from queue
    void applyStylesheet();
    void populateDitherProfileMenu();
    void clearNoiseShapingMenu();
    void on_action_DitherProfile_triggered(QAction* action, int id); // not using old "private slots:" system (just an ordinary member function.)
    void getCustomLpfParameters();
    void launch();
    void processConverterOutput(QString ConverterOutput, int channel);
    void processInputFilenames(const QStringList &fileNames);
    void loadConverterDefinitions(const QString &fileName);
    void saveConverterDefinitions(const QString &fileName) const;
    ConverterDefinition getSpecialistConverter(const QString &inExt, const QString &outExt);
    QStringList prepareSpecialistConverterArgs(const ConverterDefinition &converterDefinition, const QString &outfn, const QString &infn);
    static QString getRandomString(int length);
    QString getInfileFilter();
    QString getOutfileFilter();
    void checkConverterAvailability();
};

#endif // MAINWINDOW_H
