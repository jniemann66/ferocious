#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "outputfileoptions_dialog.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QProcess>
#include <QObject>
#include <QSettings>
#include <Qdebug>
#include <QLineEdit>
#include <QDirIterator>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QCursor>
#include <QInputDialog>
//#include <QThread>
//#include <qtconcurrentrun.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->SamplerateCombo->setCurrentText("44100");

    readSettings();

    if(ConverterPath.isEmpty()){
        ConverterPath=QDir::currentPath() + "/resampler.exe";
    }

    if(!fileExists(ConverterPath)){
        ConverterPath=QFileDialog::getOpenFileName(this,
                                                   "Please locate the file: resampler.exe",
                                                   QDir::currentPath(),
                                                   "*.exe");
    }

    if(!fileExists(ConverterPath)){
        QMessageBox msgBox;
        msgBox.setText("Unable to locate converter");
        msgBox.setInformativeText("The path to the required command-line program (resampler.exe) wasn't specified");
        msgBox.exec();
        qApp->exit();
    }

    connect(&Converter, &QProcess::readyReadStandardOutput, this, &MainWindow::on_StdoutAvailable);
    connect(&Converter, &QProcess::started, this, &MainWindow::on_ConverterStarted);
    connect(&Converter,
            static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this,
            static_cast<void(MainWindow::*)(int, QProcess::ExitStatus)>(&MainWindow::on_ConverterFinished)
    );

    this->statusBar()->setSizeGripEnabled(false);
    this->setFixedSize(size());

    // turn off the shitty etching on disabled widgets:
    QPalette pal = QApplication::palette();
    pal.setColor(QPalette::Disabled, QPalette::Text, QColor(80, 80, 80));
    pal.setColor(QPalette::Disabled, QPalette::Light, QColor(0, 0, 0, 0));
    QApplication::setPalette(pal);

    // hide context-sensitive widgets:
    ui->AutoBlankCheckBox->setEnabled(ui->DitherCheckBox->isChecked());
    ui->AutoBlankCheckBox->setVisible(ui->DitherCheckBox->isChecked());

    // get converter version:
    getResamplerVersion(ResamplerVersion);

    // set up event filter:
    qApp->installEventFilter(this);
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
    QSettings settings(QSettings::IniFormat,QSettings::SystemScope,"JuddSoft","Ferocious");

    settings.beginGroup("Paths");
    MainWindow::ConverterPath = settings.value("ConverterPath", MainWindow::ConverterPath).toString();
    MainWindow::inFileBrowsePath = settings.value("InputFileBrowsePath", MainWindow::inFileBrowsePath).toString();
    MainWindow::outFileBrowsePath = settings.value("OutputFileBrowsePath", MainWindow::outFileBrowsePath).toString();
    settings.endGroup();

    settings.beginGroup("Ui");
    ui->actionEnable_Tooltips->setChecked(settings.value("EnableToolTips",true).toBool());
    settings.endGroup();

    settings.beginGroup("CompressionSettings");
    MainWindow::flacCompressionLevel=settings.value("flacCompressionLevel", 5).toInt(); // flac default compression is 5
    MainWindow::vorbisQualityLevel=settings.value("vorbisQualityLevel", 3.0).toDouble(); // ogg vorbis default quality is 3
    settings.endGroup();

    outfileNamer.loadSettings(settings);
}

void MainWindow::writeSettings()
{
    QSettings settings(QSettings::IniFormat,QSettings::SystemScope,"JuddSoft","Ferocious");

    settings.beginGroup("Paths");
    settings.setValue("ConverterPath", MainWindow::ConverterPath);
    settings.setValue("InputFileBrowsePath",MainWindow::inFileBrowsePath);
    settings.setValue("OutputFileBrowsePath",MainWindow::outFileBrowsePath);
    settings.endGroup();

    settings.beginGroup("Ui");
    settings.setValue("EnableToolTips",ui->actionEnable_Tooltips->isChecked());
    settings.endGroup();

    settings.beginGroup("CompressionSettings");
    settings.setValue("flacCompressionLevel", MainWindow::flacCompressionLevel);
    settings.setValue("vorbisQualityLevel", MainWindow::vorbisQualityLevel);
    settings.endGroup();

    outfileNamer.saveSettings(settings);
}

void MainWindow::on_StdoutAvailable()
{
    QString ConverterOutput(Converter.readAll());
    ui->ConverterOutputText->append(ConverterOutput);
}

void MainWindow::on_ConverterStarted()
{
    ui->StatusLabel->setText("Status: Converting");
    ui->convertButton->setDisabled(true);
}

void MainWindow::on_ConverterFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    ui->StatusLabel->setText("Status: Ready");
    ui->convertButton->setEnabled(true);
}

void MainWindow::on_browseInfileButton_clicked()
{   
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Select Input File"), inFileBrowsePath, tr("Audio Files (*.aif *.aifc *.aiff *.au *.avr *.caf *.flac *.htk *.iff *.mat *.mpc *.oga *.paf *.pvf *.raw *.rf64 *.sd2 *.sds *.sf *.voc *.w64 *.wav *.wve *.xi)"));

    if(!fileName.isNull()){
        QDir path(fileName);
        inFileBrowsePath = path.absolutePath(); // remember this browse session (unix separators)
        // inFileBrowsePath = QDir::toNativeSeparators(path.absolutePath()); // remember this browse session (in native Format)
        ui->InfileEdit->setText(QDir::toNativeSeparators(fileName));

        bool bRefreshOutFilename = false;

        // trigger a refresh of outfilename if outfilename is empty and infilename is not empty:
        if(ui->OutfileEdit->text().isEmpty() && !ui->InfileEdit->text().isEmpty())
            bRefreshOutFilename = true;

        // trigger a refresh of outfilename if outfilename has a wildcard and infilename doesn't have a wildcard:
        if((ui->OutfileEdit->text().indexOf("*")>-1) && (ui->InfileEdit->text().indexOf("*")==-1))
            bRefreshOutFilename = true;

        // conditionally auto-generate output filename:
        if(bRefreshOutFilename){
            QString outFileName;
            outfileNamer.generateOutputFilename(outFileName,ui->InfileEdit->text());
            if(!outFileName.isNull() && !outFileName.isEmpty())
                ui->OutfileEdit->setText(outFileName);
                ui->OutfileEdit->update();

            // trigger an update of options if file extension changed:
            ProcessOutfileExtension();
        }
    }
}

void MainWindow::on_convertButton_clicked()
{
    // Search for Wildcards:
    if(ui->InfileEdit->text().lastIndexOf("*") > -1){ // Input Filename has wildcard

        QApplication::setOverrideCursor(Qt::BusyCursor);
        MainWindow::wildcardConvert();
        QApplication::restoreOverrideCursor();
    }

    else{ // No Wildcard:
        // Convert a Single File (easy!)
        MainWindow::convert(ui->OutfileEdit->text(), ui->InfileEdit->text());
    }
}

void MainWindow::wildcardConvert(){
    int inLastSepIndex = ui->InfileEdit->text().lastIndexOf(QDir::separator());     // position of last separator in Infile
    int outLastSepIndex = ui->OutfileEdit->text().lastIndexOf(QDir::separator());   // position of last separator in Outfile

    QString inDir;
    QString outDir;
    QString tail;

    if(inLastSepIndex >-1){
        tail = ui->InfileEdit->text().right(ui->InfileEdit->text().length()-inLastSepIndex-1); // isolate everything after last separator

        // get input directory:
        inDir = ui->InfileEdit->text().left(inLastSepIndex); // isolate directory

        // strip any wildcards out of directory name:
        inDir.replace(QString("*"),QString(""));

        // append slash to the end of Windows drive letters:
        if(inDir.length()==2 && inDir.right(1)==":"){
            qDebug() << "Drive Letter";
            inDir += "\\";
        }
    } else { // No separators in input Directory
        tail = ui->InfileEdit->text();
        inDir = "";
    }

    if(outLastSepIndex >-1){
        // get output directory:
        outDir = ui->OutfileEdit->text().left(outLastSepIndex); // isolate directory

        // strip any wildcards out of directory name:
        outDir.replace(QString("*"),QString(""));
    }
    else
        outDir="";


    qDebug() << "in Directory:" << inDir;
    qDebug() << "tail: " << tail;
    qDebug() << "out Directory" << outDir;

    QString regexString(tail);// for building a regular expression to match against filenames in the input directory.

    // convert file-system symbols to regex symbols:
    regexString.replace(QString("."),QString("\\.")); // . => \\.
    regexString.replace(QString("*"),QString(".+"));  // * => .+
    qDebug() << "Regex String: " << regexString;
    QRegularExpression regex(regexString);

    // set up an OutFileNamer for generating output file names:
    OutFileNamer O(outfileNamer); // initialize to default settings, as a fallback position.

    // initialize output directory:
    O.outputDirectory = QDir::toNativeSeparators(outDir);
    O.useSpecificOutputDirectory = true;

    // initialize output file extension:
    int outLastDot = ui->OutfileEdit->text().lastIndexOf(".");
    if(outLastDot > -1){
        O.fileExt = ui->OutfileEdit->text().right(ui->OutfileEdit->text().length()-outLastDot-1); // get file extension from file nam
        if(O.fileExt.lastIndexOf("*")>-1){ // outfile extension has a wildcard in it
            O.useSpecificFileExt = false;   // use source file extension
        }else{
            O.useSpecificFileExt = true;    // use file extension of outfile name
        }
    }else{ // outfile name has no file extension
        O.useSpecificFileExt = false; // use source file extension
    }

    // initialize output file suffix:
    // (use whatever is between last '*' and '.')
    int outLastStarBeforeDot = ui->OutfileEdit->text().left(outLastDot).lastIndexOf("*");
    if(outLastStarBeforeDot > -1){
        O.Suffix = ui->OutfileEdit->text().mid(outLastStarBeforeDot+1,outLastDot-outLastStarBeforeDot-1); // get what is between last '*' and last '.'
        O.appendSuffix = true;
    } else { // no Suffix
        O.Suffix="";
        O.appendSuffix = false;
    }

    qDebug() << "Suffix: " << O.Suffix;

    // traverse input directory
    QDirIterator it(inDir,QDir::Files); // all files in inDir

    while (it.hasNext()) {

        QString infn,outfn;
        QString nextFilename=QDir::toNativeSeparators(it.next());
        QRegularExpressionMatch match = regex.match(nextFilename);

        if (!match.hasMatch())
            continue; // no match ? move on to next file ...

        infn = QDir::toNativeSeparators(nextFilename);
        O.generateOutputFilename(outfn,infn);

        qDebug() << "InFile: " << infn;
        qDebug() << "OutFile: " << outfn;

        ui->StatusLabel->setText("Status: processing "+infn);
        this->repaint();

        MainWindow::convert(outfn, infn);
        MainWindow::Converter.waitForFinished(-1); // no timeout !
     }
}

void MainWindow::convert(const QString &outfn, const QString& infn)
{
    QStringList args;

    // format args: Main
    args << "-i" << infn << "-o" << outfn << "-r" << ui->SamplerateCombo->currentText();

    // format args: Bit Format
    if(ui->BitDepthCheckBox->isChecked()){
        args << "-b" << ui->BitDepthCombo->currentText();
    }

    // format args: Normalization
    if(ui->NormalizeCheckBox->isChecked()){
        double NormalizeAmount=ui->NormalizeAmountEdit->text().toDouble();
        if((NormalizeAmount>0.0) && (NormalizeAmount<=1.0)){
            args << "-n" << QString::number(NormalizeAmount);
        }
    }

    // format args: Double Precision
    if(ui->DoublePrecisionCheckBox->isChecked())
        args << "--doubleprecision";

    // format args: Dithering
    if(ui->DitherCheckBox->isChecked()){

        if(!ui->DitherAmountEdit->text().isEmpty()){
            double DitherAmount=ui->DitherAmountEdit->text().toDouble();
            if((DitherAmount>0.0) && (DitherAmount<=8.0)){
                args << "--dither" << QString::number(DitherAmount);
            }
        }else{
            args << "--dither";
        }

        if(ui->AutoBlankCheckBox->isChecked())
            args << "--autoblank";
    }

    // format args: Minimum Phase
    if(ui->minPhase_radioBtn->isChecked()){
        args << "--minphase";
    }

    // format compression levels for compressed formats:
    int extidx = outfn.lastIndexOf(".");
    if(extidx > -1){ // filename must have a "." to contain a file extension ...
        QString ext = outfn.right(outfn.length()-extidx-1); // get file extension from file name

        if(ext.toLower()=="flac")// format args: flac compression
            args << "--flacCompression" << QString::number(MainWindow::flacCompressionLevel);

        if(ext.toLower()=="oga") // format args: vorbis compression
            args << "--vorbisQuality" << QString::number(MainWindow::vorbisQualityLevel);
    }

    Converter.setProcessChannelMode(QProcess::MergedChannels);
    Converter.start(ConverterPath,args);
}

void MainWindow::on_InfileEdit_editingFinished()
{
    bool bRefreshOutfileEdit = false;

    // look for Wildcard in filename, before file extension
    if(ui->InfileEdit->text().indexOf("*")>-1){ // inFilename has wildcard
        int InLastDot = ui->InfileEdit->text().lastIndexOf(".");
        if(InLastDot > -1){
            int InLastStarBeforeDot = ui->InfileEdit->text().left(InLastDot).lastIndexOf("*");
            if(InLastStarBeforeDot > -1){ // Wilcard in Filename; trigger a refresh:
                bRefreshOutfileEdit = true;
              }
        }
    }

    else{ // inFilename does not have a wildcard
        if(ui->OutfileEdit->text().indexOf("*")>-1){ // outfilename does have a wildcard
            bRefreshOutfileEdit = true; // trigger a refresh
        }
    }

    if(ui->OutfileEdit->text().isEmpty() && !ui->InfileEdit->text().isEmpty())
        bRefreshOutfileEdit = true;

    if(bRefreshOutfileEdit){
        QString outFileName;

        outfileNamer.generateOutputFilename(outFileName,ui->InfileEdit->text());

        if(!outFileName.isNull() && !outFileName.isEmpty())
            ui->OutfileEdit->setText(outFileName);
            ui->OutfileEdit->update();

        // trigger an update of options if output file extension changed:
        ProcessOutfileExtension();
    }
}

void MainWindow::on_browseOutfileButton_clicked()
{
    QString path = ui->OutfileEdit->text().isEmpty() ? outFileBrowsePath : ui->OutfileEdit->text(); // if OutfileEdit is populated, use that. Otherwise, use last output file browse path

    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Select Output File"), path, tr("Audio Files (*.aiff *.au *.avr *.caf *.flac *.htk *.iff *.mat *.mpc *.oga *.paf *.pvf *.raw *.rf64 *.sd2 *.sds *.sf *.voc *.w64 *.wav *.wve *.xi)"));

    if(!fileName.isNull()){
        QDir path(fileName);
         outFileBrowsePath = path.absolutePath(); // remember this browse session (Unix separators)
       //  outFileBrowsePath = QDir::toNativeSeparators(path.absolutePath()); // remember this browse session (native separators)
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
    if(NormalizeAmount <0.0 || NormalizeAmount >1.0)
        ui->NormalizeAmountEdit->setText("1.0");
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
    if(extidx > -1){
        QString ext = fileName.right(fileName.length()-extidx-1); // get file extension from file name
        ConverterQuery.start(ConverterPath, QStringList() << "--listsubformats" << ext); // ask converter for a list of subformats for the given file extension

        if (!ConverterQuery.waitForFinished())
            return;

        ConverterQuery.setReadChannel(QProcess::StandardOutput);
        while(ConverterQuery.canReadLine()){
            QString line = QString::fromLocal8Bit(ConverterQuery.readLine());
            ui->BitDepthCombo->addItem(line.simplified());
        }
    }
}

// Query Converter for version number:
void MainWindow::getResamplerVersion(QString& v)
{
    QProcess ConverterQuery;

    ConverterQuery.start(ConverterPath, QStringList() << "--version"); // ask converter for its version number

    if (!ConverterQuery.waitForFinished())
        return;

    ConverterQuery.setReadChannel(QProcess::StandardOutput);
    while(ConverterQuery.canReadLine()){
        v += (QString::fromLocal8Bit(ConverterQuery.readLine())).simplified();
    }
}

void MainWindow::on_OutfileEdit_editingFinished()
{
   ProcessOutfileExtension(); // trigger an update of options if user changed the file extension
}

// ProcessoutFileExtension() - analyze extension of outfile and update subformats dropdown accordingly
void MainWindow::ProcessOutfileExtension()
{

    QString fileName=ui->OutfileEdit->text();
    int extidx = fileName.lastIndexOf(".");
    if(extidx > -1){ // filename must have a "." to contain a file extension ...
        QString ext = fileName.right(fileName.length()-extidx-1); // get file extension from file name

        // if user has changed the extension (ie type) of the filename, then repopulate subformats combobox:
        if(ext != lastOutputFileExt){
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
    if(DitherAmount <0.0 || DitherAmount >8.0)
        ui->DitherAmountEdit->setText("1.0");
}

void MainWindow::on_actionConverter_Location_triggered()
{
    QString cp =QFileDialog::getOpenFileName(this,
                                               "Please locate the file: resampler.exe",
                                              ConverterPath,
                                               "*.exe");
    if(!cp.isNull())
        ConverterPath = cp;
}

void MainWindow::on_actionOutput_File_Options_triggered()
{
    OutputFileOptions_Dialog D(outfileNamer);
    D.exec();
}

void MainWindow::on_actionAbout_triggered()
{
    QString info("Ferocious File Conversion\n By J.Niemann\n\n");

    info += "GUI Version: " + QString(APP_VERSION) + "\n";
    info += "Converter Vesion: " + ResamplerVersion + "\n";

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
        return QMainWindow::eventFilter(obj, event);	// pass control to base class' eventFilter
}

void MainWindow::on_actionFlac_triggered()
{
    QInputDialog D;
    D.setInputMode(QInputDialog::IntInput);
    D.setWindowTitle(tr("flac compression level"));
    D.setLabelText(tr("compression level (0-8):"));
    D.setIntMinimum(0);
    D.setIntMaximum(8);
    D.setIntValue(MainWindow::flacCompressionLevel);
    D.setIntStep(1);

    if(D.exec()==QDialog::Accepted){
        MainWindow::flacCompressionLevel = D.intValue();
    }
}

void MainWindow::on_actionOgg_Vorbis_triggered()
{
    QInputDialog D;
    D.setInputMode(QInputDialog::DoubleInput);
    D.setWindowTitle( tr("vorbis quality level"));
    D.setLabelText(tr("quality level (-1 to 10):"));
    D.setDoubleRange(-1.0,10.0);
    D.setDoubleValue(MainWindow::vorbisQualityLevel);
    D.setDoubleDecimals(2);

    if(D.exec()==QDialog::Accepted){
        MainWindow::vorbisQualityLevel = D.doubleValue();
        qDebug() << D.doubleValue();
        qDebug() << MainWindow::vorbisQualityLevel;
    }
}
