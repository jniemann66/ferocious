#include "mainwindow.h"
#include "ui_mainwindow.h"

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

    settings.beginGroup("MainWindow");
    MainWindow::ConverterPath = settings.value("ConverterPath", MainWindow::ConverterPath).toString();
    settings.endGroup();
}


void MainWindow::writeSettings()
{
    QSettings settings(QSettings::IniFormat,QSettings::SystemScope,"JuddSoft","Ferocious");

    settings.beginGroup("MainWindow");
    settings.setValue("ConverterPath", MainWindow::ConverterPath);
    settings.endGroup();
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
        tr("Select Input File"), "e:\\", tr("Audio Files (*.aif *.aifc *.aiff *.au *.avr *.caf *.flac *.htk *.iff *.mat *.mpc *.oga *.paf *.pvf *.raw *.rf64 *.sd2 *.sds *.sf *.voc *.w64 *.wav *.wve *.xi)"));

    ui->InfileEdit->setText(QDir::toNativeSeparators(fileName));

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
    }


    Converter.setProcessChannelMode(QProcess::MergedChannels);
    Converter.start(ConverterPath,args);
}

void MainWindow::on_InfileEdit_editingFinished()
{
    if(ui->OutfileEdit->text().isEmpty() && !ui->InfileEdit->text().isEmpty()){
        std::string destFilename(ui->InfileEdit->text().toStdString());         // convert to a std::string
        if(destFilename.find(".")!=std::string::npos){
            destFilename.insert(destFilename.find_last_of("."), "(converted)");     // insert "(converted)" just before file extension
            ui->OutfileEdit->setText(QString(destFilename.c_str()));
        }
    }
}

void MainWindow::on_browseOutfileButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Select Output File"), "e:\\", tr("Audio Files (*.aiff *.au *.avr *.caf *.flac *.htk *.iff *.mat *.mpc *.oga *.paf *.pvf *.raw *.rf64 *.sd2 *.sds *.sf *.voc *.w64 *.wav *.wve *.xi)"));

    ui->OutfileEdit->setText(QDir::toNativeSeparators(fileName));
    PopulateBitFormats(ui->OutfileEdit->text());
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

void MainWindow::on_OutfileEdit_editingFinished()
{
    PopulateBitFormats(ui->OutfileEdit->text());
}

void MainWindow::on_DitherCheckBox_clicked()
{
    ui->DitherAmountEdit->setEnabled(ui->DitherCheckBox->isChecked());
}

void MainWindow::on_DitherAmountEdit_editingFinished()
{
    double DitherAmount = ui->DitherAmountEdit->text().toDouble();
    if(DitherAmount <0.0 || DitherAmount >8.0)
        ui->DitherAmountEdit->setText("1.0");
}
