#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "outputfileoptions_dialog.h"
#include "fancylineedit.h"
#include "lpfparametersdlg.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QProcess>
#include <QObject>
#include <QSettings>
#include <QDebug>
#include <QLineEdit>
#include <QDirIterator>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QCursor>
#include <QInputDialog>
#include <QStringList>
#include <QClipboard>
#include <QTextStream>
#include <QScroller>
#include <QScrollBar>

#define RECURSIVE_DIR_TRAVERSAL

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), launchType(LaunchType::Convert)
{
    ui->setupUi(this);
    ui->SamplerateCombo->setCurrentText("44100");

    browseInMenu = new QMenu(this);
    browseInMenu->setHidden(true);
    convertTaskMenu = new QMenu(this);
    convertTaskMenu->setHidden(true);

    readSettings();
    applyStylesheet(); // note: no-op if file doesn't exist, or file is factory default (":/ferocious.css")

    if(ConverterPath.isEmpty()) {
        ConverterPath=QDir::currentPath() + "/" + expectedConverter; // attempt to find converter in currentPath
    }

    if(!fileExists(ConverterPath)) {
        QString s("Please locate the file: ");
        s.append(expectedConverter);

#if defined (Q_OS_WIN)
        QString filter = "*.exe";
#else
        QString filter = "";
#endif

        ConverterPath=QFileDialog::getOpenFileName(this,
                                                   s,
                                                   QDir::currentPath(),
                                                   filter);

        if(ConverterPath.lastIndexOf(expectedConverter, -1, Qt::CaseInsensitive) == -1) { // safeguard against wrong executable being configured
            ConverterPath.clear();
            QMessageBox::warning(this, tr("Converter Location"), tr("That is not the right program!\n"), QMessageBox::Ok);
        }
    }

    if(!fileExists(ConverterPath)) {
        QMessageBox msgBox;
        QString s("The path to the required command-line program (");
        s.append(expectedConverter);
        s.append(") wasn't specified");
        msgBox.setText("Unable to locate converter");
        msgBox.setInformativeText(s);
        msgBox.exec();
        qApp->exit();
    }

    // turn off the shitty etching on disabled widgets:
    QPalette pal = QApplication::palette();
    pal.setColor(QPalette::Disabled, QPalette::Text, QColor(80, 80, 80));
    pal.setColor(QPalette::Disabled, QPalette::Light, QColor(0, 0, 0, 0));
    QApplication::setPalette(pal);

    // hide context-sensitive widgets:
    ui->AutoBlankCheckBox->setEnabled(ui->DitherCheckBox->isChecked());
    ui->AutoBlankCheckBox->setVisible(ui->DitherCheckBox->isChecked());
    ui->OutfileEdit->hideEditButton();
    ui->progressBar->setVisible(false);
    ui->statusBar->setVisible(false);

    // get converter version:
    getResamplerVersion();

    // retrieve dither profiles and add them to menu:
    populateDitherProfileMenu();

    // set up event filter:
    qApp->installEventFilter(this);

    // Set the separator for Multiple-files:
    MultiFileSeparator = "\n";

    // Add kinetic scroller to converter output
    QScroller::grabGesture(ui->ConverterOutputText, QScroller::LeftMouseButtonGesture);

    connect(&converter, &QProcess::readyReadStandardOutput, this, &MainWindow::on_StdoutAvailable);
    connect(&converter, &QProcess::readyReadStandardError, this, &MainWindow::on_StderrAvailable);
    connect(&converter, &QProcess::started, this, &MainWindow::on_ConverterStarted);
    connect(&converter,
            static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this,
            static_cast<void(MainWindow::*)(int, QProcess::ExitStatus)>(&MainWindow::on_ConverterFinished)
    );
    connect(ui->convertButton, &flashingPushbutton::rightClicked, this, &MainWindow::on_convertButton_rightClicked);
    connect(ui->convertButton, &flashingPushbutton::stopRequested, this, &MainWindow::on_stopRequested);
    connect(ui->browseInfileButton, &flashingPushbutton::rightClicked, this, &MainWindow::on_browseInButton_rightClicked);
}

MainWindow::~MainWindow()
{
    writeSettings();
    delete ui;
}

bool MainWindow::fileExists(const QString& path) {
    QFileInfo fi(path);
    return (fi.exists() && fi.isFile());
}

void MainWindow::readSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "JuddSoft", "Ferocious");

    qDebug() << "Reading settings file: " << settings.fileName();

    settings.beginGroup("Paths");
    MainWindow::ConverterPath = settings.value("ConverterPath", MainWindow::ConverterPath).toString();
    if(ConverterPath.lastIndexOf(expectedConverter, -1, Qt::CaseInsensitive) == -1) { // safeguard against wrong executable being configured
        ConverterPath.clear();
    }
    MainWindow::inFileBrowsePath = settings.value("InputFileBrowsePath", MainWindow::inFileBrowsePath).toString();
    MainWindow::outFileBrowsePath = settings.value("OutputFileBrowsePath", MainWindow::outFileBrowsePath).toString();
    settings.endGroup();

    settings.beginGroup("Ui");
    ui->actionEnable_Tooltips->setChecked(settings.value("EnableToolTips", true).toBool());
    MainWindow::stylesheetFilePath = settings.value("StylesheetPath", ":/ferocious.css").toString();
    settings.endGroup();

    settings.beginGroup("CompressionSettings");
    MainWindow::flacCompressionLevel=settings.value("flacCompressionLevel", 5).toInt(); // flac default compression is 5
    MainWindow::vorbisQualityLevel=settings.value("vorbisQualityLevel", 3.0).toDouble(); // ogg vorbis default quality is 3
    settings.endGroup();

    settings.beginGroup("ConversionSettings");
    MainWindow::bDisableClippingProtection=settings.value("disableClippingProtection", false).toBool();
    MainWindow::bEnableMultithreading=settings.value("enableMultithreading", true).toBool();
    MainWindow::bSingleStage=settings.value("singleStage", false).toBool();
    MainWindow::bNoTempFile=settings.value("noTempFile", false).toBool();

    // Note: "on/off" options are to be stored the way they are used in ReSampler's commandline.
    // However, in the UI, the options may be represented in the inverse form. eg:
    // "Enable Clipping Protection" (UI) => !disableClippingProtection

    ui->actionEnable_Clipping_Protection->setChecked(!bDisableClippingProtection);
    ui->actionEnable_Multi_Threading->setChecked(bEnableMultithreading);
    ui->actionMultiStageConversion->setChecked(!bSingleStage);
    ui->actionUse_a_temp_file->setChecked(!bNoTempFile);

    settings.endGroup();

    settings.beginGroup("LPFSettings");
    MainWindow::LPFtype = static_cast<LPFType>(settings.value("LPFtype", 1).toInt());
    ui->actionRelaxedLPF->setChecked(false);
    ui->actionStandardLPF->setChecked(false);
    ui->actionSteepLPF->setChecked(false);
    ui->actionCustomLPF->setChecked(false);

    switch(LPFtype) {
    case relaxedLPF:
         ui->actionRelaxedLPF->setChecked(true);
        break;
    case steepLPF:
        ui->actionSteepLPF->setChecked(true);
        break;
    case customLPF:
        ui->actionCustomLPF->setChecked(true);
        ui->actionCustom_Parameters->setVisible(true);
        break;
    default:
         ui->actionStandardLPF->setChecked(true);
    }

    MainWindow::customLpfCutoff = settings.value("customLpfCutoff", 95.45).toDouble();
    MainWindow::customLpfTransition = settings.value("customLpfTransition", 4.55).toDouble();
    settings.endGroup();

    settings.beginGroup("advancedDitherSettings");
    MainWindow::bFixedSeed=settings.value("fixedSeed", false).toBool();
    ui->actionFixed_Seed->setChecked(MainWindow::bFixedSeed);
    MainWindow::seedValue=settings.value("seedValue", 0).toInt();
    MainWindow::noiseShape = static_cast<NoiseShape>(settings.value("noiseShape", noiseShape_standard).toInt());
    MainWindow::ditherProfile=settings.value("ditherProfile", -1).toInt();
    clearNoiseShapingMenu();
    if(ditherProfile == -1 /* none */) {
        if(noiseShape == noiseShape_flatTpdf)
             ui->actionNoiseShapingFlatTpdf->setChecked(true);
        else
            ui->actionNoiseShapingStandard->setChecked(true);
    }
    settings.endGroup();
    filenameGenerator.loadSettings(settings);
}

void MainWindow::writeSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "JuddSoft", "Ferocious");

    settings.beginGroup("Paths");
    settings.setValue("ConverterPath", MainWindow::ConverterPath);
    settings.setValue("InputFileBrowsePath", MainWindow::inFileBrowsePath);
    settings.setValue("OutputFileBrowsePath", MainWindow::outFileBrowsePath);
    settings.endGroup();

    settings.beginGroup("Ui");
    settings.setValue("EnableToolTips", ui->actionEnable_Tooltips->isChecked());
    settings.setValue("StylesheetPath", MainWindow::stylesheetFilePath);
    settings.endGroup();

    settings.beginGroup("CompressionSettings");
    settings.setValue("flacCompressionLevel", MainWindow::flacCompressionLevel);
    settings.setValue("vorbisQualityLevel", MainWindow::vorbisQualityLevel);
    settings.endGroup();

    settings.beginGroup("ConversionSettings");
    settings.setValue("disableClippingProtection", MainWindow::bDisableClippingProtection);
    settings.setValue("enableMultithreading", MainWindow::bEnableMultithreading);
    settings.setValue("singleStage", MainWindow::bSingleStage);
    settings.setValue("noTempFile", MainWindow::bNoTempFile);
    settings.endGroup();

    settings.beginGroup("LPFSettings");
    settings.setValue("LPFtype", MainWindow::LPFtype);
    settings.setValue("customLpfCutoff", MainWindow::customLpfCutoff);
    settings.setValue("customLpfTransition", MainWindow::customLpfTransition);
    settings.endGroup();

    settings.beginGroup("advancedDitherSettings");
    settings.setValue("fixedSeed", MainWindow::bFixedSeed);
    settings.setValue("seedValue", MainWindow::seedValue);
    settings.setValue("noiseShape", MainWindow::noiseShape);
    settings.setValue("ditherProfile", MainWindow::ditherProfile);
    settings.endGroup();

    filenameGenerator.saveSettings(settings);
}

void MainWindow::on_StderrAvailable()
{
     processConverterOutput(converter.readAllStandardError(), 2);
}

void MainWindow::on_StdoutAvailable()
{
    processConverterOutput(converter.readAllStandardOutput(), 1);
}

void MainWindow::processConverterOutput(QString ConverterOutput, int channel) {
    int progress = 0;

    // count backspaces at end of string:
    int backspaces = 0;
    while(ConverterOutput.at(ConverterOutput.length() - 1) == '\b') {
        ConverterOutput.chop(1);
        ++backspaces;
    }

    if(backspaces) {
        // extract percentage:
        QString whatToChop = ConverterOutput.right(backspaces);
        if(whatToChop.indexOf("%") != -1) {
            progress = whatToChop.replace("%", "").toInt();
            ui->progressBar->setValue(progress);
        }
        ConverterOutput.chop(backspaces);
    }

    if(!ConverterOutput.isEmpty()) {
        if(channel == 2 ) {
            ui->ConverterOutputText->append("<font color=\"red\">" + ConverterOutput + "</font>");
        } else {
            ui->ConverterOutputText->append(ConverterOutput);
        }
        ui->ConverterOutputText->verticalScrollBar()->triggerAction(QScrollBar::SliderToMaximum);
    }
}

void MainWindow::on_ConverterStarted()
{
    ui->convertButton->setIsActive(true);
    ui->progressBar->setValue(0);
    if(bShowProgressBar)
        ui->progressBar->setVisible(true);
}

void MainWindow::on_ConverterFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);

    if(!MainWindow::conversionQueue.isEmpty()) {
        MainWindow::convertNext();
        ui->progressBar->setValue(0);
    } else {
        ui->progressBar->setVisible(false);
        ui->StatusLabel->setText("Status: Ready");
        ui->convertButton->setIsActive(false);
    }
}

void MainWindow::on_browseInButton_rightClicked()
{
    for (auto& a : browseInMenu->actions()) {
        browseInMenu->removeAction(a);
    }

    browseInMenu->addAction("Select Files ...", this, &MainWindow::on_browseInfileButton_clicked);
    browseInMenu->addAction("Select Entire Directory ...", [this] {
        QFileDialog fileDialog(this);
        fileDialog.setDirectory(inFileBrowsePath);
        fileDialog.setFileMode(QFileDialog::Directory);
        fileDialog.setViewMode(QFileDialog::Detail);

        if(fileDialog.exec()) {
            QString fName = QDir::toNativeSeparators(fileDialog.selectedFiles().first() + "/*");
            if(!fName.isEmpty())
                processInputFilenames(QStringList{fName});
        }
    });

    browseInMenu->popup(QCursor::pos());
}

void MainWindow::on_browseInfileButton_clicked()
{   
    QFileDialog fileDialog(this);
    fileDialog.setDirectory(inFileBrowsePath);
    fileDialog.setFileMode(QFileDialog::ExistingFiles);
    fileDialog.setNameFilter("Audio Files (*.aif *.aifc *.aiff *.au *.avr *.caf *.dff *.dsf *.flac *.htk *.iff *.mat *.mpc *.oga *.paf *.pvf *.raw *.rf64 *.sd2 *.sds *.sf *.voc *.w64 *.wav *.wve *.xi)");
    fileDialog.setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if(fileDialog.exec()) {
        fileNames = fileDialog.selectedFiles();
    } else {
        return;
    }

    processInputFilenames(fileNames);
}

void MainWindow::processInputFilenames(const QStringList& fileNames) {

    QString filenameSpec;

    if(fileNames.isEmpty())
        return;

    if(fileNames.count() >= 1) {
        QDir path(fileNames.first());
        inFileBrowsePath = path.absolutePath(); // record path of this browse session
    }

    if(fileNames.count() == 1) { // Single-file mode:
        ui->InfileLabel->setText("Input File:");
        ui->OutfileLabel->setText("Output File:");
        ui->OutfileEdit->setReadOnly(false);
        filenameSpec=fileNames.first();

        if(!filenameSpec.isEmpty()) {
            ui->InfileEdit->setText(QDir::toNativeSeparators(filenameSpec));
            bool bRefreshOutFilename = true;

            // trigger a refresh of outfilename if outfilename is empty and infilename is not empty:
            if(ui->OutfileEdit->text().isEmpty() && !ui->InfileEdit->text().isEmpty())
                bRefreshOutFilename = true;

            // trigger a refresh of outfilename if outfilename has a wildcard and infilename doesn't have a wildcard:
            if((ui->OutfileEdit->text().indexOf("*") > -1) && (ui->InfileEdit->text().indexOf("*") == -1))
                bRefreshOutFilename = true;

            // conditionally auto-generate output filename:
            if(bRefreshOutFilename) {
                QString outFileName;
                filenameGenerator.generateOutputFilename(outFileName,ui->InfileEdit->text());
                if(!outFileName.isNull() && !outFileName.isEmpty())
                    ui->OutfileEdit->setText(outFileName);
                    ui->OutfileEdit->update();
            }
        }

    } else { // Multiple-file Mode:

        // actually change label to say "Input Files:" instead of "Input File:"
        ui->InfileLabel->setText("Input Files:");

        // Convert each input filename to native separators,
        // and join all the strings together with MultiFileSeparator in between each string
        QStringList::const_iterator it;
        for (it = fileNames.constBegin(); it != fileNames.constEnd(); ++it) {
            filenameSpec += QDir::toNativeSeparators(*it);
            filenameSpec += MultiFileSeparator;
        }

        // set widget text. Note: MultiFileSeparator should be a non-displayable character, but still be picked-up in copy/paste
        ui->InfileEdit->setText(filenameSpec);

        // determine output filename
        QString outFilename = QDir::toNativeSeparators(fileNames.first()); // use first input filename as a basis for generating output filename
        int LastDot = outFilename.lastIndexOf(".");
        int LastSep = outFilename.lastIndexOf(QDir::separator());
        QString s = outFilename.mid(LastSep + 1, LastDot - LastSep - 1); // get what is between last separator and last '.'
        if(!s.isEmpty() && !s.isNull()) {
            outFilename.replace(s, "*"); // replace everything between last separator and file extension with a wildcard ('*'):
        }
        filenameGenerator.generateOutputFilename(outFilename,outFilename); // Generate output filename by applying name-generation rules

        // update the output filename widget and label
        ui->OutfileEdit->setText(outFilename);
        ui->OutfileEdit->update();
        ui->OutfileLabel->setText("Output Files: (filenames auto-generated)");
        ui->OutfileEdit->setReadOnly(true);
    }

    // trigger an update of options if file extension changed:
    ProcessOutfileExtension();
}

void MainWindow::on_convertButton_clicked()
{
    launchType = LaunchType::Convert;
    launch();
}

void MainWindow::launch() {
    QStringList filenames=ui->InfileEdit->text().split(MultiFileSeparator);

    // iterate over the filenames, adding either a single conversion, or wildcard conversion at each iteration
    for (const QString& inFilename : filenames) {

        if(!inFilename.isEmpty() && !inFilename.isNull()) {

            // Search for Wildcards:
            if(inFilename.lastIndexOf("*") > -1) { // Input Filename has wildcard
                MainWindow::wildcardPushToQueue(inFilename);
            }

            else { // No Wildcard:
                conversionTask T;
                T.inFilename = inFilename;
                if(filenames.count() > 1) { // multi-file mode:
                    filenameGenerator.generateOutputFilename(T.outFilename, inFilename);
                } else { // single-file mode:
                    T.outFilename = ui->OutfileEdit->text();
                }

                MainWindow::conversionQueue.push_back(T);
            }
        }
    }
    MainWindow::convertNext();
}

// wildcardPushtoQueue() - expand wildcard in filespec, and push matching filenames into queue:
void MainWindow::wildcardPushToQueue(const QString& inFilename) {
    int inLastSepIndex = inFilename.lastIndexOf(QDir::separator());     // position of last separator in Infile
    int outLastSepIndex = ui->OutfileEdit->text().lastIndexOf(QDir::separator());   // position of last separator in Outfile

    QString inDir;
    QString outDir;
    QString tail;

    if(inLastSepIndex > -1) {
        tail = inFilename.right(inFilename.length() - inLastSepIndex - 1); // isolate everything after last separator

        // get input directory:
        inDir = inFilename.left(inLastSepIndex); // isolate directory

        // strip any wildcards out of directory name:
        inDir.replace(QString("*"), QString(""));

        // append slash to the end of Windows drive letters:
        if(inDir.length() == 2 && inDir.right(1) == ":")
            inDir += "\\";

    } else { // No separators in input Directory
        tail = inFilename;
        inDir = "";
    }

    if(outLastSepIndex >-1) {
        // get output directory:
        outDir = ui->OutfileEdit->text().left(outLastSepIndex); // isolate directory

        // strip any wildcards out of directory name:
        outDir.replace(QString("*"), QString(""));
    }
    else
        outDir = "";

    QString regexString(tail); // for building a regular expression to match against filenames in the input directory.

    // convert file-system symbols to regex symbols:
    regexString.replace(QString("."), QString("\\.")); // . => \\.
    regexString.replace(QString("*"), QString(".+"));  // * => .+
    QRegularExpression regex(regexString);

    // set up a FilenameGenerator for generating output file names:
    FilenameGenerator O(filenameGenerator); // initialize to default settings, as a fallback position.

    // initialize output directory:
    O.outputDirectory = QDir::toNativeSeparators(outDir);
    O.useSpecificOutputDirectory = true;

    // initialize output file extension:
    int outLastDot = ui->OutfileEdit->text().lastIndexOf(".");
    if(outLastDot > -1) {
        O.fileExt = ui->OutfileEdit->text().right(ui->OutfileEdit->text().length() - outLastDot - 1); // get file extension from file nam
        if(O.fileExt.lastIndexOf("*") > -1) { // outfile extension has a wildcard in it
            O.useSpecificFileExt = false;   // use source file extension
        } else {
            O.useSpecificFileExt = true;    // use file extension of outfile name
        }
    } else{ // outfile name has no file extension
        O.useSpecificFileExt = false; // use source file extension
    }

    // initialize output file suffix:
    // (use whatever is between last '*' and '.')
    int outLastStarBeforeDot = ui->OutfileEdit->text().left(outLastDot).lastIndexOf("*");
    if(outLastStarBeforeDot > -1) {
        O.Suffix = ui->OutfileEdit->text().mid(outLastStarBeforeDot + 1, outLastDot-outLastStarBeforeDot - 1); // get what is between last '*' and last '.'
        O.appendSuffix = true;
    } else { // no Suffix
        O.Suffix = "";
        O.appendSuffix = false;
    }

    // traverse input directory

    QStringList createdDirectoriesList;

#ifdef RECURSIVE_DIR_TRAVERSAL
    QDirIterator it(inDir, QDir::Files, QDirIterator::Subdirectories);
#else
    QDirIterator it(inDir, QDir::Files); // all files in inDir
#endif

    while (it.hasNext()) {

        QString nextFilename = QDir::toNativeSeparators(it.next());
        QRegularExpressionMatch match = regex.match(nextFilename);

        if (!match.hasMatch())
            continue; // no match ? move on to next file ...

        conversionTask T;
        T.inFilename = QDir::toNativeSeparators(nextFilename);

#ifdef RECURSIVE_DIR_TRAVERSAL

        // get (just the) name of the inDir folder
        QString dirName = QDir(inDir).dirName();

        // assemble subdirectory path as follows: dirName/path-to/nextFilename
        QString sd = dirName + "/" + QDir(inDir).relativeFilePath(nextFilename) /* filePath of subdirectory relative to inDir */;

        // get just the filename
        QFileInfo fi(nextFilename);
        QString fn = fi.fileName();

        // chop filename and final separator from sd
        // eg: dirName/path-to
        sd.chop(sd.length() - sd.lastIndexOf(fn) + 1);

        if(!sd.isEmpty()) {

            // create output subdirectory if it doesn't already exist
            QDir dir(QDir::toNativeSeparators(O.outputDirectory + "/" + sd));
            QString p(dir.absolutePath());

            if(ui->actionMock_Conversion->isChecked()) { // mock-create directory
                if(!createdDirectoriesList.contains(p)) {
                    ui->ConverterOutputText->append("<font color=\"yellow\">mkdir \"" + QDir::toNativeSeparators(dir.absolutePath()) + "\"</font>");
                    createdDirectoriesList.append(p);
                }

                // simulate time-delay of converter
                QTimer::singleShot(10, [this] {
                    on_ConverterFinished(0, QProcess::NormalExit);
                });
            }

            else { // create directory for real
                if (!dir.exists()) {
                    ui->ConverterOutputText->append("<font color=\"yellow\">mkdir " + QDir::toNativeSeparators(dir.absolutePath()) + "</font>");
                    dir.mkpath(".");
                }
            }
        }

        O.generateOutputFilename(T.outFilename, T.inFilename, QDir::toNativeSeparators(sd));
#else
        O.generateOutputFilename(T.outFilename, T.inFilenam);
#endif

        MainWindow::conversionQueue.push_back(T);
     }
}

// convertNext() - take the next conversion task from the front of the queue, convert it, then remove it from queue.

void MainWindow::convertNext() {
    if(!conversionQueue.empty()) {
        conversionTask& nextTask = MainWindow::conversionQueue.first();
        ui->StatusLabel->setText("Status: processing " + nextTask.inFilename);
        ui->progressBar->setFormat("Status: processing " + nextTask.inFilename);
        this->repaint();
        MainWindow::convert(nextTask.outFilename, nextTask.inFilename);
        conversionQueue.removeFirst();
    }
}

// convert() - the function that actually launches the converter
// converts file infn to outfn using current parameters

void MainWindow::convert(const QString &outfn, const QString& infn)
{
    QStringList args;

    // format args: Main
    args << "-i" << infn << "-o" << outfn << "-r" << ui->SamplerateCombo->currentText();

    // format args: Bit Format
    if(ui->BitDepthCheckBox->isChecked()) {
        args << "-b" << ui->BitDepthCombo->currentText();
    }

    // format args: Normalization
    if(ui->NormalizeCheckBox->isChecked()) {
        double NormalizeAmount=ui->NormalizeAmountEdit->text().toDouble();
        if((NormalizeAmount > 0.0) && (NormalizeAmount <= 1.0)) {
            args << "-n" << QString::number(NormalizeAmount);
        }
    }

    // format args: Double Precision
    if(ui->DoublePrecisionCheckBox->isChecked())
        args << "--doubleprecision";

    // format args: Dithering
    if(ui->DitherCheckBox->isChecked()) {

        if(!ui->DitherAmountEdit->text().isEmpty()) {
            double DitherAmount=ui->DitherAmountEdit->text().toDouble();
            if((DitherAmount > 0.0) && (DitherAmount <= 8.0)) {
                args << "--dither" << QString::number(DitherAmount);
            }
        } else {
            args << "--dither";
        }

        if(ui->AutoBlankCheckBox->isChecked())
            args << "--autoblank";

        // format args: dither profile:
        if(MainWindow::ditherProfile != -1) {
            args << "--ns" << QString::number(MainWindow::ditherProfile);
        }

        // format args: noise-shaping
        else if(MainWindow::noiseShape == noiseShape_flatTpdf) {
            args << "--flat-tpdf";
        }

        // format args: seed
        if(MainWindow::bFixedSeed) {
            args << "--seed" << QString::number(MainWindow::seedValue);
        }
    }

    // format args: Minimum Phase
    if(ui->minPhase_radioBtn->isChecked()) {
        args << "--minphase";
    }

    // format compression levels for compressed formats:
    int extidx = outfn.lastIndexOf(".");
    if(extidx > -1) { // filename must have a "." to contain a file extension ...
        QString ext = outfn.right(outfn.length() - extidx - 1); // get file extension from file name

        if(ext.toLower() == "flac")// format args: flac compression
            args << "--flacCompression" << QString::number(MainWindow::flacCompressionLevel);

        if(ext.toLower() == "oga") // format args: vorbis compression
            args << "--vorbisQuality" << QString::number(MainWindow::vorbisQualityLevel);
    }

    // format args: --noClippingProtection
    if(bDisableClippingProtection) {
        args << "--noClippingProtection";
    }

    // format args: --mt
    if(bEnableMultithreading) {
        args << "--mt";
    }

    // format args: --singleStage
    if(bSingleStage) {
        args << "--singleStage";
    }

    // format args: --noTempFile
    if(bNoTempFile) {
        args << "--noTempFile";
    }

    // format args: LPF type:
    switch(LPFtype) {
    case relaxedLPF:
        args << "--relaxedLPF";
        break;
    case steepLPF:
        args << "--steepLPF";
        break;
    case customLPF:
        args << "--lpf-cutoff" << QString::number(customLpfCutoff);
        args << "--lpf-transition" << QString::number(customLpfTransition);
        break;
    default:
        break;
    }

    // wrap args in quotes if necessary:
    // 1. spaces definitely need quotes
    // 2. parentheses and backslashes, and probably a whole lot of other characters, can cause problems with bash

    QStringList quotedArgs;
    for(QString& arg : args) {
        quotedArgs.append(arg.contains(QRegExp("[() \\\\]")) ? "\"" + arg + "\"" : arg);
    }

    if(launchType == LaunchType::Convert) {

        if(ui->actionMock_Conversion->isChecked()) {
            ui->ConverterOutputText->append("<font color=\"orange\">" + QDir::toNativeSeparators(ConverterPath) + " " + quotedArgs.join(" ") + "</font>");
            QTimer::singleShot(25, [this] {
                on_ConverterFinished(0, QProcess::NormalExit);
            });
        }
        else {
            converter.setProcessChannelMode(QProcess::SeparateChannels);
            converter.start(ConverterPath, args);
        }
    }

    else if(launchType == LaunchType::Clipboard) {

       // get current clipboard text and append new line to it:
       QString clipText = QGuiApplication::clipboard()->text();
       QTextStream out(&clipText);
       out << QDir::toNativeSeparators(ConverterPath);
       out << " ";
       out << quotedArgs.join(" ");
       out << "\n";
       QGuiApplication::clipboard()->setText(clipText);

       QTimer::singleShot(20, [this] {
           on_ConverterFinished(0, QProcess::NormalExit);
       });
    }
}

void MainWindow::on_InfileEdit_editingFinished()
{
    QString inFilename = ui->InfileEdit->text();

    if(inFilename.isEmpty()) {

        // reset to single file mode:
        ui->InfileLabel->setText("Input File:");
        ui->OutfileLabel->setText("Output File:");
        ui->OutfileEdit->setReadOnly(false);

        // reset outFilename
        ui->OutfileEdit->clear();

        return;
    }

    bool bRefreshOutfileEdit = true; // control whether to always update output filename

    // look for Wildcard in filename, before file extension
    if(inFilename.indexOf("*") > -1) { // inFilename has wildcard
        int InLastDot =inFilename.lastIndexOf(".");
        if(InLastDot > -1) {
            int InLastStarBeforeDot = inFilename.left(InLastDot).lastIndexOf("*");
            if(InLastStarBeforeDot > -1) { // Wilcard in Filename; trigger a refresh:
                bRefreshOutfileEdit = true;
            }
        }
    }

    else { // inFilename does not have a wildcard
        if(ui->OutfileEdit->text().indexOf("*") > -1) { // outfilename does have a wildcard
            bRefreshOutfileEdit = true; // trigger a refresh
        }
    }

    if(ui->OutfileEdit->text().isEmpty() && !ui->InfileEdit->text().isEmpty())
        bRefreshOutfileEdit = true;

    QString outFilename;

    if(inFilename.right(1) == MultiFileSeparator) {
        inFilename=inFilename.left(inFilename.size() - 1); // Trim Multifile separator off the end
        ui->InfileEdit->setText(inFilename);
    }

    if(inFilename.indexOf(MultiFileSeparator) == -1) { // Single-file mode:

        ui->InfileLabel->setText("Input File:");
        ui->OutfileLabel->setText("Output File:");
        ui->OutfileEdit->setReadOnly(false);

        if(bRefreshOutfileEdit) {
            filenameGenerator.generateOutputFilename(outFilename, ui->InfileEdit->text());
            if(!outFilename.isNull() && !outFilename.isEmpty())
                ui->OutfileEdit->setText(outFilename);
            ui->OutfileEdit->update();
        }
    }

    else { // multi-file mode:

        QString outFilename=inFilename.left(inFilename.indexOf(MultiFileSeparator)); // use first filename as a basis for generating output filename
        int LastDot = outFilename.lastIndexOf(".");
        int LastSep = outFilename.lastIndexOf(QDir::separator());
        QString s = outFilename.mid(LastSep + 1, LastDot - LastSep - 1); // get what is between last separator and last '.'
        if(!s.isEmpty() && !s.isNull()) {
            outFilename.replace(s, "*"); // replace everything between last separator and file extension with a wildcard ('*'):
        }
        filenameGenerator.generateOutputFilename(outFilename, outFilename); // Generate output filename by applying name-generation rules
        ui->OutfileEdit->setText(outFilename);
        ui->OutfileLabel->setText("Output Files: (filenames auto-generated)");
        ui->OutfileEdit->setReadOnly(true);
        ui->OutfileEdit->update();
        ui->InfileLabel->setText("Input Files:");
    }

    // trigger an update of options if output file extension changed:
    ProcessOutfileExtension();
}

void MainWindow::on_browseOutfileButton_clicked()
{
    QString path = ui->OutfileEdit->text().isEmpty() ? outFileBrowsePath : ui->OutfileEdit->text(); // if OutfileEdit is populated, use that. Otherwise, use last output file browse path

    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Select Output File"), path, tr("Audio Files (*.aiff *.au *.avr *.caf *.dff *.dsf *.flac *.htk *.iff *.mat *.mpc *.oga *.paf *.pvf *.raw *.rf64 *.sd2 *.sds *.sf *.voc *.w64 *.wav *.wve *.xi)"));

    if(!fileName.isNull()) {
        QDir path(fileName);
        outFileBrowsePath = path.absolutePath(); // remember this browse session (Unix separators)
        ui->OutfileEdit->setText(QDir::toNativeSeparators(fileName));
        PopulateBitFormats(ui->OutfileEdit->text());

        // trigger an update of options if file extension changed:
        ProcessOutfileExtension();
    }
}

void MainWindow::on_NormalizeCheckBox_clicked()
{
    ui->NormalizeAmountEdit->setEnabled(ui->NormalizeCheckBox->isChecked());
}

void MainWindow::on_NormalizeAmountEdit_editingFinished()
{
    double NormalizeAmount = ui->NormalizeAmountEdit->text().toDouble();
    if(NormalizeAmount < 0.0 || NormalizeAmount > 1.0)
        ui->NormalizeAmountEdit->setText("1.00");
}

void MainWindow::on_BitDepthCheckBox_clicked()
{
    ui->BitDepthCombo->setEnabled(ui->BitDepthCheckBox->isChecked());
}

// Launch external process, and populate QComboBox using output from the process:
void MainWindow::PopulateBitFormats(const QString& fileName)
{
    QProcess ConverterQuery;
    ui->BitDepthCombo->clear();
    int extidx = fileName.lastIndexOf(".");
    if(extidx > -1) {
        QString ext = fileName.right(fileName.length() - extidx - 1); // get file extension from file name
        ConverterQuery.start(ConverterPath, QStringList() << "--listsubformats" << ext); // ask converter for a list of subformats for the given file extension

        if (!ConverterQuery.waitForFinished())
            return;

        ConverterQuery.setReadChannel(QProcess::StandardOutput);
        while(ConverterQuery.canReadLine()) {
            QString line = QString::fromLocal8Bit(ConverterQuery.readLine());
            ui->BitDepthCombo->addItem(line.simplified());
        }
    }
}

// Query Converter for version number:
void MainWindow::getResamplerVersion()
{
    QString v;
    QProcess ConverterQuery;
    ConverterQuery.start(ConverterPath, QStringList() << "--version"); // ask converter for its version number

    if (!ConverterQuery.waitForFinished())
        return;

    ConverterQuery.setReadChannel(QProcess::StandardOutput);
    while(ConverterQuery.canReadLine()) {
        v += (QString::fromLocal8Bit(ConverterQuery.readLine())).simplified();

        // split the version number into components:
        QStringList ResamplerVersionNumbers = v.split(".");

        // set various options accoring to resampler version:
        int vA=ResamplerVersionNumbers[0].toInt(); // 1st number
        int vB=ResamplerVersionNumbers[1].toInt(); // 2nd number

        bShowProgressBar = (vA > 1) || (vB >= 1); // (no progress output on ReSampler versions prior to 1.1.0)
        resamplerVersion=v;
    }
}

void MainWindow::on_OutfileEdit_editingFinished()
{
   ProcessOutfileExtension(); // trigger an update of options if user changed the file extension
}

// ProcessoutFileExtension() - analyze extension of outfile and update subformats dropdown accordingly
void MainWindow::ProcessOutfileExtension()
{

    QString fileName = ui->OutfileEdit->text();
    int extidx = fileName.lastIndexOf(".");
    if(extidx > -1) { // filename must have a "." to contain a file extension ...
        QString ext = fileName.right(fileName.length() - extidx - 1); // get file extension from file name

        // if user has changed the extension (ie type) of the filename, then repopulate subformats combobox:
        if(ext != lastOutputFileExt) {
            PopulateBitFormats(fileName);
            lastOutputFileExt=ext;
        }
    }
}

void MainWindow::on_DitherCheckBox_clicked()
{
    ui->DitherAmountEdit->setEnabled(ui->DitherCheckBox->isChecked());
    ui->AutoBlankCheckBox->setEnabled(ui->DitherCheckBox->isChecked());
    ui->AutoBlankCheckBox->setVisible(ui->DitherCheckBox->isChecked());
}

void MainWindow::on_DitherAmountEdit_editingFinished()
{
    double DitherAmount = ui->DitherAmountEdit->text().toDouble();
    if(DitherAmount < 0.0 || DitherAmount > 8.0)
        ui->DitherAmountEdit->setText("1.0");
}

void MainWindow::on_actionConverter_Location_triggered()
{
    QString s("Please locate the file: ");
    s.append(expectedConverter);

#if defined (Q_OS_WIN)
    QString filter = "*.exe";
#else
    QString filter = "";
#endif

    QString cp =QFileDialog::getOpenFileName(this, s, ConverterPath,  filter);

    if(!cp.isNull()) {
        ConverterPath = cp;
        if(ConverterPath.lastIndexOf(expectedConverter, -1, Qt::CaseInsensitive) == -1) { // safeguard against wrong executable being configured
            ConverterPath.clear();
            QMessageBox::warning(this, tr("Converter Location"), tr("That is not the right program!\n"), QMessageBox::Ok);
        } else {
            // get converter version:
            getResamplerVersion();
        }
    }
}

void MainWindow::on_actionOutput_File_Options_triggered()
{
    OutputFileOptions_Dialog d(filenameGenerator);
    d.exec();
    on_InfileEdit_editingFinished(); // trigger change of output file if relevant
}

void MainWindow::on_actionAbout_triggered()
{
    QString info("Ferocious File Conversion\n By J.Niemann\n\n");

    info += "GUI Version: " + QString(APP_VERSION) + "\n";
    info += "Converter Vesion: " + resamplerVersion + "\n";

    QMessageBox msgBox;
    msgBox.setText("About");
    msgBox.setInformativeText(info);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setIconPixmap(QPixmap(":/images/sine_sweep-32x32-buttonized.png"));
    msgBox.exec();
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QApplication::aboutQt();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::ToolTip) // Intercept tooltip event
        return (!ui->actionEnable_Tooltips->isChecked());

    else
        return QMainWindow::eventFilter(obj, event);
}

void MainWindow::on_actionFlac_triggered()
{
    QInputDialog d;
    d.setInputMode(QInputDialog::IntInput);
    d.setWindowTitle(tr("flac compression level"));
    d.setLabelText(tr("compression level (0-8):"));
    d.setIntMinimum(0);
    d.setIntMaximum(8);
    d.setIntValue(MainWindow::flacCompressionLevel);
    d.setIntStep(1);

    if(d.exec() == QDialog::Accepted)
        MainWindow::flacCompressionLevel = d.intValue();
}

void MainWindow::on_actionOgg_Vorbis_triggered()
{
    QInputDialog d;
    d.setInputMode(QInputDialog::DoubleInput);
    d.setWindowTitle( tr("vorbis quality level"));
    d.setLabelText(tr("quality level (-1 to 10):"));
    d.setDoubleRange(-1.0, 10.0);
    d.setDoubleValue(MainWindow::vorbisQualityLevel);
    d.setDoubleDecimals(2);

    if(d.exec() == QDialog::Accepted)
        MainWindow::vorbisQualityLevel = d.doubleValue();
}

void MainWindow::on_actionEnable_Clipping_Protection_triggered()
{
    bDisableClippingProtection = !ui->actionEnable_Clipping_Protection->isChecked();
}

void MainWindow::applyStylesheet() {

    if(stylesheetFilePath.isEmpty())
        stylesheetFilePath = ":/ferocious.css"; // factory default

    if(stylesheetFilePath == ":/ferocious.css") {
        qDebug() << "using factory default theme";
        return;
    }

    if(!fileExists(stylesheetFilePath)) {
        qDebug() << "stylesheet " << stylesheetFilePath << " doesn't exist";
        return;
    }

    QApplication* a = qApp;

    // retrieve and apply Stylesheet:
    QFile ss(stylesheetFilePath);
    if(ss.open(QIODevice::ReadOnly | QIODevice::Text)) {
        a->setStyleSheet(ss.readAll());
        ss.close();
    } else {
        qDebug() << "Couldn't open stylesheet resource " << stylesheetFilePath;
    }
}

void MainWindow::on_actionTheme_triggered()
{
    stylesheetFilePath = QFileDialog::getOpenFileName(this, "Choose a Stylesheet", QDir::currentPath(), tr("Style Sheets (*.css *.css)"));
    applyStylesheet();
}

void MainWindow::on_actionRelaxedLPF_triggered()
{
    LPFtype = relaxedLPF;
    ui->actionRelaxedLPF->setChecked(true);
    ui->actionStandardLPF->setChecked(false);
    ui->actionSteepLPF->setChecked(false);
    ui->actionCustomLPF->setChecked(false);
    ui->actionCustom_Parameters->setVisible(false);
}

void MainWindow::on_actionStandardLPF_triggered()
{
    LPFtype = standardLPF;
    ui->actionRelaxedLPF->setChecked(false);
    ui->actionStandardLPF->setChecked(true);
    ui->actionSteepLPF->setChecked(false);
    ui->actionCustomLPF->setChecked(false);
    ui->actionCustom_Parameters->setVisible(false);
}

void MainWindow::on_actionSteepLPF_triggered()
{
    LPFtype = steepLPF;
    ui->actionRelaxedLPF->setChecked(false);
    ui->actionStandardLPF->setChecked(false);
    ui->actionSteepLPF->setChecked(true);
    ui->actionCustomLPF->setChecked(false);
    ui->actionCustom_Parameters->setVisible(false);
}

void MainWindow::on_actionCustomLPF_triggered()
{
    LPFtype = customLPF;
    ui->actionRelaxedLPF->setChecked(false);
    ui->actionStandardLPF->setChecked(false);
    ui->actionSteepLPF->setChecked(false);
    if(ui->actionCustomLPF->isChecked()) {
        getCustomLpfParameters();
    }
    ui->actionCustomLPF->setChecked(true);
    ui->actionCustom_Parameters->setVisible(true);
}

void MainWindow::on_actionFixed_Seed_triggered()
{
    bFixedSeed = ui->actionFixed_Seed->isChecked();
}

void MainWindow::on_actionSeed_Value_triggered()
{
    QInputDialog d;
    d.setInputMode(QInputDialog::IntInput);
    d.setWindowTitle(tr("Choose Seed for Random Number Generator"));
    d.setLabelText(tr("Seed (-2,147,483,648 to 2,147,483,647):"));
    d.setIntMinimum(-2147483647 - 1); // note: compiler warning if you initialize with -2147483648 (because it tries to start with 2147483648 and then apply a unary minus)
    d.setIntMaximum(2147483647);
    d.setIntValue(MainWindow::seedValue);
    d.setIntStep(1);

    if(d.exec() == QDialog::Accepted)
        MainWindow::seedValue = d.intValue();
}

void MainWindow::on_actionEnable_Multi_Threading_triggered(bool checked)
{
    MainWindow::bEnableMultithreading = checked;
}

/*
note regarding Noise Shaping and Dither Profiles:
"Noise Shaping" refers to either standard or flat-tpdf
The two menu items "Standard" and "Flat TPDF" are always present.
"Dither Profile" refers to dither profiles to be issued with the --ns option
Dither Profiles were introduced much later in the development of ReSampler.
The additional "dither profiles" are only added to the menu if the version of ReSampler being used
has the capability.
*/

void MainWindow::on_actionNoiseShapingStandard_triggered()
{
    MainWindow::noiseShape = noiseShape_standard;
    clearNoiseShapingMenu();
    ui->actionNoiseShapingStandard->setChecked(true);
    MainWindow::ditherProfile = -1; // none
}

void MainWindow::on_actionNoiseShapingFlatTpdf_triggered()
{
    MainWindow::noiseShape = noiseShape_flatTpdf;
    clearNoiseShapingMenu();
    ui->actionNoiseShapingFlatTpdf->setChecked(true);
    MainWindow::ditherProfile = -1; // none
}

void MainWindow::on_action_DitherProfile_triggered(QAction* action, int id)
{
    clearNoiseShapingMenu();
    action->setChecked(true);
    ditherProfile = id;
}

void MainWindow::clearNoiseShapingMenu()
{
    QList<QAction*> nsActions = ui->menuNoise_Shaping->actions();
    for(int i = 0; i < nsActions.count(); ++i )
    {
        nsActions[i]->setChecked(false);
    }
}

void MainWindow::populateDitherProfileMenu()
{

    QList<int> ignoreList;
    ignoreList << 0 << 6; // dither profiles to not add to menu (flat, standard)

    QMenu* nsMenu = ui->menuNoise_Shaping;

    // Launch external process, and populate Menu using output from the process:
    QProcess ConverterQuery;
    ConverterQuery.start(ConverterPath, QStringList() << "--showDitherProfiles");
    if (!ConverterQuery.waitForFinished() || (ConverterQuery.exitCode() != 0)) {
        // note: earlier versions of ReSampler that don't understand --showDitherProfiles
        // are expected to return exitCode of 1
        return;
    }

    ConverterQuery.setReadChannel(QProcess::StandardOutput);
    while(ConverterQuery.canReadLine()) {
        QString line = QString::fromLocal8Bit(ConverterQuery.readLine());
        QStringList fields = line.split(":");
        int id = fields.at(0).toInt();
        if(ignoreList.indexOf(id) == -1) {
            QString label = fields.at(1).simplified();
            QAction* action = nsMenu->addAction(label);
            action->setCheckable(true);
            action->setChecked(id == ditherProfile);
            connect(action, &QAction::triggered, this, [=]() {
                this->on_action_DitherProfile_triggered(action, id);
            });
        }
    }
}

void MainWindow::on_actionCustom_Parameters_triggered()
{
    getCustomLpfParameters();
}

void MainWindow::getCustomLpfParameters() {
    auto d = new lpfParametersDlg(this);
    d->setValues(customLpfCutoff, customLpfTransition);
    d->setWindowTitle("Custom LPF Parameters");
    connect(d, &QDialog::accepted, this, [this, d] {
        auto v = d->getValues();
        customLpfCutoff = v.first;
        customLpfTransition = v.second;
    });
    d->exec();
}

void MainWindow::on_convertButton_rightClicked() {
   for (auto& a : convertTaskMenu->actions()) {
       convertTaskMenu->removeAction(a);
   }

   convertTaskMenu->addAction("Convert", this, &MainWindow::on_convertButton_clicked);
   convertTaskMenu->addAction("Copy Command Line to Clipboard", [this] {
        QGuiApplication::clipboard()->clear();
        launchType = LaunchType::Clipboard;
        launch();
   });

   convertTaskMenu->popup(QCursor::pos());
}

void MainWindow::on_stopRequested() {
    qDebug() << "STOP!";
    conversionQueue.clear();
    converter.kill();
    ui->StatusLabel->setText("Status: conversion stopped");
}

void MainWindow::on_actionMultiStageConversion_triggered(bool checked)
{
    MainWindow::bSingleStage = !checked;
}

void MainWindow::on_actionUse_a_temp_file_triggered(bool checked)
{
    bNoTempFile = !checked;
}
