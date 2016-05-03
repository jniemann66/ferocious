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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->SamplerateCombo->setCurrentText("44100");

    readSettings();

    if(ConverterPath.isEmpty()){
        ConverterPath=QDir::currentPath() + "/resampler.exe";
    // qDebug() << ConverterPath;
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
    qDebug() << ResamplerVersion;

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

        // conditionally auto-generate output filename:
        if(ui->OutfileEdit->text().isEmpty() && !ui->InfileEdit->text().isEmpty()){
            QString outFileName;
            outfileNamer.generateOutputFilename(outFileName,ui->InfileEdit->text());
            if(!outFileName.isNull() && !outFileName.isEmpty())
                ui->OutfileEdit->setText(outFileName);
                ui->OutfileEdit->update();
        }
    }
}

void MainWindow::on_convertButton_clicked()
{

    QStringList args;

    // format args: Main
    args << "-i" << ui->InfileEdit->text() << "-o" << ui->OutfileEdit->text() << "-r" << ui->SamplerateCombo->currentText();

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


    Converter.setProcessChannelMode(QProcess::MergedChannels);
    Converter.start(ConverterPath,args);
}

void MainWindow::on_InfileEdit_editingFinished()
{
    if(ui->OutfileEdit->text().isEmpty() && !ui->InfileEdit->text().isEmpty()){
        QString outFileName;
        outfileNamer.generateOutputFilename(outFileName,ui->InfileEdit->text());
        if(!outFileName.isNull() && !outFileName.isEmpty())
            ui->OutfileEdit->setText(outFileName);
            ui->OutfileEdit->update();
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
    // if user has changed the extension (ie type) of the filename, then repopulate subformats combobox:
    QString fileName=ui->OutfileEdit->text();
    int extidx = fileName.lastIndexOf(".");
    if(extidx > -1){ // filename must have a "." to contain a file extension ...
        QString ext = fileName.right(fileName.length()-extidx-1); // get file extension from file name
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
    msgBox.setPalette(this->palette());
    msgBox.setIconPixmap(QPixmap(":/images/sine_sweep-32x32-buttonized.png"));
    msgBox.setStyleSheet(
                "QPushButton {\
                color: #fff;\
                border: 1px solid #1b2018;\
                border-radius: 9px;\
                padding: 5px;\
                background: qradialgradient(cx: 0.3, cy: -0.4,\
                fx: 0.3, fy: -0.4,\
                radius: 1.35, stop: 0 #46503f, stop: 1 #2d3328);\
                min-width: 66px;\
                }\
                QPushButton:hover {\
                background: qradialgradient(cx: 0.3, cy: -0.4,\
                fx: 0.3, fy: -0.4,\
                radius: 1.35, stop: 0 #535e4a, stop: 1 #3a4234);\
                }\
                QPushButton:pressed {\
                background: qradialgradient(cx: 0.4, cy: -0.1,\
                fx: 0.4, fy: -0.1,\
                radius: 1.35, stop: 0 #70e01a, stop: 1#3b770e);\
                }");
    \
    msgBox.exec();
}
