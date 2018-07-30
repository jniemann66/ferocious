#include "outputfileoptions_dialog.h"
#include "ui_outputfileoptions_dialog.h"

#include <QFileDialog>

FilenameGenerator::FilenameGenerator() {
    // factory defaults:
    appendSuffix=true;
    suffix=QString("(converted)");
    inputDirectoryRoot=QString();
    useSpecificOutputDirectory=false;
    outputDirectory=QString("");
    useSpecificFileExt=false;
    fileExt=QString("wav");
    replicateDirectoryStructure = false;
}

FilenameGenerator::FilenameGenerator(const FilenameGenerator &O)
    : appendSuffix(O.appendSuffix),
      useSpecificOutputDirectory(O.useSpecificOutputDirectory),
      replicateDirectoryStructure(O.replicateDirectoryStructure),
      useSpecificFileExt(O.useSpecificFileExt),
      suffix(O.suffix),
      inputDirectoryRoot(O.inputDirectoryRoot),
      outputDirectory(O.outputDirectory),
      fileExt(O.fileExt)
{
    /*---*/
}

void FilenameGenerator::generateOutputFilename(QString &outFilename, const QString &inFilename, const QString subDirectory /* = QString() */) {

    outFilename = inFilename;

    // if outFilename contains a wildcard, replace everything between last separator and file extension with a wildcard ('*'):
    int outLastDot = outFilename.lastIndexOf(".");
    if((inFilename.indexOf("*") > -1) && (outLastDot > -1)) {
        int outLastStarBeforeDot = outFilename.left(outLastDot).lastIndexOf(QDir::separator());
        if(outLastStarBeforeDot > -1) {
            QString s = outFilename.mid(outLastStarBeforeDot + 1, outLastDot-outLastStarBeforeDot - 1); // get what is between last '*' and last '.'
            if(!s.isEmpty() && !s.isNull()) {
                outFilename.replace(s,"*");
            }
        }
    }

    std::string strOutFilename = outFilename.toStdString();  // std::string version of outFilename (start with copy of inFilename)
    std::string sep(QString(QDir::separator()).toStdString()); // separator: '\' for windows, '/' for 'nix

    if(useSpecificOutputDirectory) {
        if(!inFilename.isEmpty()) {
            if(strOutFilename.find(sep)!=std::string::npos) { // replace input file path with user's output directory:
                if(subDirectory.isEmpty()) {
                    strOutFilename = outputDirectory.toStdString()
                        + sep
                        + strOutFilename.substr(strOutFilename.find_last_of(sep) + 1, strOutFilename.length() - 1);
                } else {
                    strOutFilename = outputDirectory.toStdString()
                        + sep
                        + subDirectory.toStdString()
                        + sep
                        + strOutFilename.substr(strOutFilename.find_last_of(sep) + 1, strOutFilename.length() - 1);
                }
            }
        }
    }

    // conditionally append suffix to filename:
    if(appendSuffix) {
        if(!inFilename.isEmpty()) {
            if(strOutFilename.find(".") != std::string::npos) {
                strOutFilename.insert(strOutFilename.find_last_of("."), suffix.toStdString());     // insert suffix just before file extension
            }
        }
    }

    // conditionally change file extension:
    if(useSpecificFileExt) {
        if (strOutFilename.find_last_of(".") != std::string::npos) { // has an extension
            strOutFilename = strOutFilename.substr(0,strOutFilename.find_last_of("."))+ "." + fileExt.toStdString(); // replace extension with fileExt
        }
        else { // doesn't have an extension
            strOutFilename.append("." + fileExt.toStdString()); // append fileExt
        }
    }

    outFilename = QString(strOutFilename.c_str()); // convert std::string back to QString
}

void FilenameGenerator::saveSettings(QSettings &settings)
{
    settings.beginGroup("OutputFileOptions");
    settings.setValue("appendSuffix", FilenameGenerator::appendSuffix);
    settings.setValue("Suffix", FilenameGenerator::suffix);
    settings.setValue("useSpecificOutputDirectory", FilenameGenerator::useSpecificOutputDirectory);
    settings.setValue("outputDirectory", QDir(FilenameGenerator::outputDirectory).absolutePath());
    settings.setValue("useSpecificFileExt", FilenameGenerator::useSpecificFileExt);
    settings.setValue("fileExt", FilenameGenerator::fileExt);
    settings.endGroup();
}

void FilenameGenerator::loadSettings(QSettings &settings)
{
    settings.beginGroup("OutputFileOptions");
    FilenameGenerator::appendSuffix = settings.value("appendSuffix", FilenameGenerator::appendSuffix).toBool();
    FilenameGenerator::suffix = settings.value("Suffix", FilenameGenerator::suffix).toString();
    FilenameGenerator::useSpecificOutputDirectory = settings.value("useSpecificOutputDirectory", FilenameGenerator::useSpecificOutputDirectory).toBool();
    FilenameGenerator::outputDirectory = QDir::toNativeSeparators(settings.value("outputDirectory",  FilenameGenerator::outputDirectory).toString());
    FilenameGenerator::useSpecificFileExt = settings.value("useSpecificFileExt", FilenameGenerator::useSpecificFileExt).toBool();
    FilenameGenerator::fileExt = settings.value("fileExt", FilenameGenerator::fileExt).toString();
    settings.endGroup();
}

OutputFileOptions_Dialog::OutputFileOptions_Dialog(FilenameGenerator& filenameGenerator, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OutputFileOptions_Dialog)
{
    ui->setupUi(this);
    pFilenameGenerator = &filenameGenerator; // keep a pointer to caller's referenced object

    // populate controls using members of OFN object
    ui->FilenameSuffix_checkBox->setChecked(pFilenameGenerator->appendSuffix);
    ui->outFilenameSuffix_lineEdit->setText(pFilenameGenerator->suffix);
    ui->useOutputDirectory_checkBox->setChecked(pFilenameGenerator->useSpecificOutputDirectory);
    ui->outDirectory_lineEdit->setText(pFilenameGenerator->outputDirectory);
    ui->SameFileExt_radioButton->setChecked(!pFilenameGenerator->useSpecificFileExt);
    ui->setFileExt_radioButton->setChecked(pFilenameGenerator->useSpecificFileExt);
    ui->outFileExt_lineEdit->setText(pFilenameGenerator->fileExt);

    // enable relevant lineEdit boxes:
    ui->outFilenameSuffix_lineEdit->setEnabled(pFilenameGenerator->appendSuffix);
    ui->outDirectory_lineEdit->setEnabled(pFilenameGenerator->useSpecificOutputDirectory);
    ui->outFileExt_lineEdit->setEnabled(pFilenameGenerator->useSpecificFileExt);
}

OutputFileOptions_Dialog::~OutputFileOptions_Dialog()
{
    delete ui;
}

void OutputFileOptions_Dialog::on_FilenameSuffix_checkBox_clicked()
{
    ui->outFilenameSuffix_lineEdit->setEnabled(ui->FilenameSuffix_checkBox->isChecked());
}

void OutputFileOptions_Dialog::on_useOutputDirectory_checkBox_clicked()
{
    ui->outDirectory_lineEdit->setEnabled(ui->useOutputDirectory_checkBox->isChecked());
}

void OutputFileOptions_Dialog::on_setFileExt_radioButton_clicked()
{
    ui->outFileExt_lineEdit->setEnabled(ui->setFileExt_radioButton->isChecked());
}

void OutputFileOptions_Dialog::on_SameFileExt_radioButton_clicked()
{
    ui->outFileExt_lineEdit->setEnabled(ui->setFileExt_radioButton->isChecked());
}

void OutputFileOptions_Dialog::on_OutputFileOptions_buttonBox_accepted()
{
    // suffix settings:
    pFilenameGenerator->appendSuffix=ui->FilenameSuffix_checkBox->isChecked();
    pFilenameGenerator->suffix=ui->outFilenameSuffix_lineEdit->text();

    // Output Directory Settings:
    pFilenameGenerator->useSpecificOutputDirectory=ui->useOutputDirectory_checkBox->isChecked();
    pFilenameGenerator->outputDirectory=ui->outDirectory_lineEdit->text();
    if(pFilenameGenerator->outputDirectory.right(1) == QDir::separator())
        pFilenameGenerator->outputDirectory = pFilenameGenerator->outputDirectory.left(pFilenameGenerator->outputDirectory.length()-1); // remove separator from end of string

    // File extension Settings:
    pFilenameGenerator->useSpecificFileExt=ui->setFileExt_radioButton->isChecked();
    pFilenameGenerator->fileExt=ui->outFileExt_lineEdit->text();
    if( pFilenameGenerator->fileExt.left(1) == ".")
        pFilenameGenerator->fileExt = pFilenameGenerator->fileExt.right(pFilenameGenerator->fileExt.length()-1); // remove leading "." from file extension
}

void OutputFileOptions_Dialog::on_pushButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Output Directory"),
                                                   ui->outDirectory_lineEdit->text(),
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if(!dir.isNull() && !dir.isEmpty())
        ui->outDirectory_lineEdit->setText(QDir::toNativeSeparators(dir));

}
