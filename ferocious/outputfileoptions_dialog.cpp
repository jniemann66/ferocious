#include "outputfileoptions_dialog.h"
#include "ui_outputfileoptions_dialog.h"
#include <QFileDialog>


OutFileNamer::OutFileNamer(){
    // factory defaults:
    appendSuffix=true;
    Suffix=QString("(converted)");
    useSpecificOutputDirectory=false;
    outputDirectory=QString("");
    useSpecificFileExt=false;
    fileExt=QString("wav");
}

OutFileNamer::OutFileNamer(const OutFileNamer &O)
    : appendSuffix(O.appendSuffix),
      Suffix(O.Suffix),
      useSpecificOutputDirectory(O.useSpecificOutputDirectory),
      outputDirectory(O.outputDirectory),
      useSpecificFileExt(O.useSpecificFileExt),
      fileExt(O.fileExt)

{
    /*---*/
}

void OutFileNamer::generateOutputFilename(QString &outFilename, const QString &inFilename){

    outFilename = inFilename;

    // if outFilename contains a wildcard, replace everything between last separator and file extension with a wildcard ('*'):
    int outLastDot = outFilename.lastIndexOf(".");
    if((inFilename.indexOf("*")>-1) && (outLastDot > -1)){
        int outLastStarBeforeDot = outFilename.left(outLastDot).lastIndexOf(QDir::separator());
        if(outLastStarBeforeDot > -1){
            QString s = outFilename.mid(outLastStarBeforeDot+1,outLastDot-outLastStarBeforeDot-1); // get what is between last '*' and last '.'
            if(!s.isEmpty() && !s.isNull()){
                outFilename.replace(s,"*");
            }
        }
    }

    std::string strOutFilename = outFilename.toStdString();  // std::string version of outFilename (start with copy of inFilename)
    std::string sep(QString(QDir::separator()).toStdString()); // separator: '\' for windows, '/' for 'nix

    // conditionally replace input file path with user's output directory:
    if(useSpecificOutputDirectory){
        if(!inFilename.isEmpty()){
            if(strOutFilename.find(sep)!=std::string::npos){
                strOutFilename = outputDirectory.toStdString() + sep + strOutFilename.substr(strOutFilename.find_last_of(sep)+1, strOutFilename.length()-1);
            }
        }
    }

    // conditionally append suffix to filename:
    if(appendSuffix){
        if(!inFilename.isEmpty()){
            if(strOutFilename.find(".")!=std::string::npos){
                strOutFilename.insert(strOutFilename.find_last_of("."), Suffix.toStdString());     // insert suffix just before file extension
            }
        }
    }

    // conditionally change file extension:
    if(useSpecificFileExt){
        if (strOutFilename.find_last_of(".") != std::string::npos){ // has an extension
            strOutFilename = strOutFilename.substr(0,strOutFilename.find_last_of("."))+ "." + fileExt.toStdString(); // replace extension with fileExt
        }
        else{ // doesn't have an extension
            strOutFilename.append("." + fileExt.toStdString()); // append fileExt
        }
    }

    outFilename=(QString(strOutFilename.c_str())); // convert std::string back to QString
}

void OutFileNamer::saveSettings(QSettings &settings)
{
    settings.beginGroup("OutputFileOptions");

    settings.setValue("appendSuffix", OutFileNamer::appendSuffix);
    settings.setValue("Suffix", OutFileNamer::Suffix);
    settings.setValue("useSpecificOutputDirectory", OutFileNamer::useSpecificOutputDirectory);
    settings.setValue("outputDirectory", QDir(OutFileNamer::outputDirectory).absolutePath());
    settings.setValue("useSpecificFileExt", OutFileNamer::useSpecificFileExt);
    settings.setValue("fileExt", OutFileNamer::fileExt);

    settings.endGroup();
}

void OutFileNamer::loadSettings(QSettings &settings)
{
    settings.beginGroup("OutputFileOptions");

    OutFileNamer::appendSuffix = settings.value("appendSuffix", OutFileNamer::appendSuffix).toBool();
    OutFileNamer::Suffix = settings.value("Suffix", OutFileNamer::Suffix).toString();
    OutFileNamer::useSpecificOutputDirectory = settings.value("useSpecificOutputDirectory", OutFileNamer::useSpecificOutputDirectory).toBool();
    OutFileNamer::outputDirectory = QDir::toNativeSeparators(settings.value("outputDirectory",  OutFileNamer::outputDirectory).toString());
    OutFileNamer::useSpecificFileExt = settings.value("useSpecificFileExt", OutFileNamer::useSpecificFileExt).toBool();
    OutFileNamer::fileExt = settings.value("fileExt", OutFileNamer::fileExt).toString();

    settings.endGroup();
}

//

OutputFileOptions_Dialog::OutputFileOptions_Dialog(OutFileNamer& OFN, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OutputFileOptions_Dialog)
{

    ui->setupUi(this);

    pOutFileNamer = &OFN; // keep a pointer to caller's referenced object

    // populate controls using members of OFN object
    ui->FilenameSuffix_checkBox->setChecked(pOutFileNamer->appendSuffix);
    ui->outFilenameSuffix_lineEdit->setText(pOutFileNamer->Suffix);
    ui->useOutputDirectory_checkBox->setChecked(pOutFileNamer->useSpecificOutputDirectory);
    ui->outDirectory_lineEdit->setText(pOutFileNamer->outputDirectory);
    ui->SameFileExt_radioButton->setChecked(!pOutFileNamer->useSpecificFileExt);
    ui->setFileExt_radioButton->setChecked(pOutFileNamer->useSpecificFileExt);
    ui->outFileExt_lineEdit->setText(pOutFileNamer->fileExt);

    // enable relevant lineEdit boxes:
    ui->outFilenameSuffix_lineEdit->setEnabled(pOutFileNamer->appendSuffix);
    ui->outDirectory_lineEdit->setEnabled(pOutFileNamer->useSpecificOutputDirectory);
    ui->outFileExt_lineEdit->setEnabled(pOutFileNamer->useSpecificFileExt);

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
    pOutFileNamer->appendSuffix=ui->FilenameSuffix_checkBox->isChecked();
    pOutFileNamer->Suffix=ui->outFilenameSuffix_lineEdit->text();

    // Output Directory Settings:
    pOutFileNamer->useSpecificOutputDirectory=ui->useOutputDirectory_checkBox->isChecked();
    pOutFileNamer->outputDirectory=ui->outDirectory_lineEdit->text();
    if(pOutFileNamer->outputDirectory.right(1)==QDir::separator())
        pOutFileNamer->outputDirectory = pOutFileNamer->outputDirectory.left(pOutFileNamer->outputDirectory.length()-1); // remove separator from end of string

    // File extension Settings:
    pOutFileNamer->useSpecificFileExt=ui->setFileExt_radioButton->isChecked();
    pOutFileNamer->fileExt=ui->outFileExt_lineEdit->text();
    if( pOutFileNamer->fileExt.left(1)==".")
        pOutFileNamer->fileExt = pOutFileNamer->fileExt.right(pOutFileNamer->fileExt.length()-1); // remove leading "." from file extension
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
