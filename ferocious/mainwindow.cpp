/*
* Copyright (C) 2016 - 2023 Judd Niemann - All Rights Reserved.
* You may use, distribute and modify this code under the
* terms of the GNU Lesser General Public License, version 2.1
*
* You should have received a copy of GNU Lesser General Public License v2.1
* with this file. If not, please refer to: https://github.com/jniemann66/ferocious
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "outputfileoptions_dialog.h"
#include "fancylineedit.h"
#include "lpfparametersdlg.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
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
#include <QJsonObject>
#include <QJsonDocument>
#include <QTime>
#include <QRandomGenerator>

#define RECURSIVE_DIR_TRAVERSAL

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->SamplerateCombo->setCurrentText("44100");

	browseInMenu = new QMenu(this);
	browseInMenu->setHidden(true);
	browseOutMenu = new QMenu(this);
	browseOutMenu->setHidden(true);
	convertTaskMenu = new QMenu(this);
	converterConfigurationDialog = new ConverterConfigurationDialog(this, Qt::Window);
	convertTaskMenu->setHidden(true);

	readSettings();

	//    applyStylesheet(); // note: no-op if file doesn't exist, or file is factory default (":/ferocious.css")

	if(converterPath.isEmpty()) {
		converterPath=QDir::currentPath() + "/" + expectedConverter; // attempt to find converter in currentPath
	}

	if(!fileExists(converterPath)) {
		QString s("Please locate the file: ");
		s.append(expectedConverter);

#if defined (Q_OS_WIN)
		QString filter = "*.exe";
#else
		QString filter = "";
#endif

		converterPath = QFileDialog::getOpenFileName(this, s, QDir::currentPath(), filter);

		if(converterPath.lastIndexOf(expectedConverter, -1, Qt::CaseInsensitive) == -1) { // safeguard against wrong executable being configured
			converterPath.clear();
			QMessageBox::warning(this, tr("Converter Location"), tr("That is not the right program!\n"), QMessageBox::Ok);
		}
	}

	if(!fileExists(converterPath)) {
		QMessageBox msgBox;
		QString s = QStringLiteral("The path to the command-line program (%1) wasn't specified").arg(expectedConverter);
		msgBox.setText(tr("Unable to locate converter"));
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
	queryResamplerVersion();
	queryResamplerSndfileVersion();

	// retrieve dither profiles and add them to menu:
	populateDitherProfileMenu();

	// set up event filter:
	qApp->installEventFilter(this);

	// Set the separator for Multiple-files:
	multiFileSeparator = "\n";

	// Add kinetic scroller to converter output
	QScroller::grabGesture(ui->ConverterOutputText->viewport(), QScroller::LeftMouseButtonGesture);

	if(converterDefinitions.isEmpty()) {
		// load factory converter definitions
		loadConverterDefinitions(":/converters.json");
	}

	connect(&process, &QProcess::readyReadStandardOutput, this, &MainWindow::on_StdoutAvailable);
	connect(&process, &QProcess::readyReadStandardError, this, &MainWindow::on_StderrAvailable);
	connect(&process, &QProcess::started, this, &MainWindow::on_ConverterStarted);
	connect(&process,
			static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
			this,
			static_cast<void(MainWindow::*)(int, QProcess::ExitStatus)>(&MainWindow::on_ConverterFinished)
			);
	connect(ui->convertButton, &FlashingPushButton::stopRequested, this, &MainWindow::on_stopRequested);
	connect(ui->convertButton, &FlashingPushButton::rightClicked, this, &MainWindow::onConvertButtonRightClicked);
	connect(ui->browseInfileButton, &FlashingPushButton::rightClicked, this, &MainWindow::onBrowseInButtonRightClicked);
	connect(ui->browseOutfileButton, &FlashingPushButton::rightClicked, this, &MainWindow::onBrowseOutButtonRightClicked);
}

MainWindow::~MainWindow()
{
	writeSettings();
	delete ui;
}

bool MainWindow::fileExists(const QString& path)
{
	QFileInfo fi(path);
	return (fi.exists() && fi.isFile());
}

void MainWindow::readSettings()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "JuddSoft", "Ferocious");

	qDebug() << "Reading settings file: " << settings.fileName();

	settings.beginGroup("WindowGeometry");
	if(converterConfigurationDialog != nullptr) {
		QRect converterConfigDialogGeometry = settings.value("ConverterConfigurationDialogGeometry").toRect();
		if(!converterConfigDialogGeometry.isNull()) {
			converterConfigurationDialog->setGeometry(converterConfigDialogGeometry);
		}
		QRect converterConfigEditDialogGeometry = settings.value("ConverterConfigurationEditDialogGeometry").toRect();
		if(!converterConfigEditDialogGeometry.isNull()) {
			converterConfigurationDialog->setEditDialogGeometry(converterConfigEditDialogGeometry);
		}
	}
	settings.endGroup();

	settings.beginGroup("Paths");
	MainWindow::converterPath = settings.value("ConverterPath", MainWindow::converterPath).toString();
	if(converterPath.lastIndexOf(expectedConverter, -1, Qt::CaseInsensitive) == -1) { // safeguard against wrong executable being configured
		converterPath.clear();
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
	MainWindow::lpfType = static_cast<LPFType>(settings.value("LPFtype", 1).toInt());
	ui->actionRelaxedLPF->setChecked(false);
	ui->actionStandardLPF->setChecked(false);
	ui->actionSteepLPF->setChecked(false);
	ui->actionCustomLPF->setChecked(false);

	switch(lpfType) {
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

	QString converterDefinitionsFilename =
			QDir::toNativeSeparators(QFileInfo(settings.fileName()).path() + "/" + "converters.json");
	loadConverterDefinitions(converterDefinitionsFilename);
}

void MainWindow::writeSettings()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "JuddSoft", "Ferocious");

	settings.beginGroup("WindowGeometry");
	if(converterConfigurationDialog != nullptr) {
		settings.setValue("ConverterConfigurationDialogGeometry", converterConfigurationDialog->geometry());
		if(!converterConfigurationDialog->getEditDialogGeometry().isNull()) {
			settings.setValue("ConverterConfigurationEditDialogGeometry", converterConfigurationDialog->getEditDialogGeometry());
		}
	}
	settings.endGroup();

	settings.beginGroup("Paths");
	settings.setValue("ConverterPath", MainWindow::converterPath);
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
	settings.setValue("LPFtype", MainWindow::lpfType);
	settings.setValue("customLpfCutoff", MainWindow::customLpfCutoff);
	settings.setValue("customLpfTransition", MainWindow::customLpfTransition);
	settings.endGroup();

	settings.beginGroup("advancedDitherSettings");
	settings.setValue("fixedSeed", MainWindow::bFixedSeed);
	settings.setValue("seedValue", MainWindow::seedValue);
	settings.setValue("noiseShape", MainWindow::noiseShape);
	settings.setValue("ditherProfile", MainWindow::ditherProfile);
	settings.endGroup();

	if(!converterDefinitions.isEmpty()) {
		QString converterDefinitionsFilename =
				QDir::toNativeSeparators(QFileInfo(settings.fileName()).path() + "/" + "converters.json");
		saveConverterDefinitions(converterDefinitionsFilename);
	}

	filenameGenerator.saveSettings(settings);
}

void MainWindow::on_StderrAvailable()
{
	processConverterOutput(process.readAllStandardError(), 2);
}

void MainWindow::on_StdoutAvailable()
{
	processConverterOutput(process.readAllStandardOutput(), 1);
}

void MainWindow::processConverterOutput(QString converterOutput, int channel)
{	
	// capture progress updates
	static const QRegularExpression progressRx("\\d+%[\\b]+");
	auto rxMatches = progressRx.globalMatch(converterOutput);
	QStringList progressUpdates;
	while(rxMatches.hasNext()) {
		progressUpdates.append(rxMatches.next().captured(0));
	}

	if(!progressUpdates.isEmpty()) {
		// Use last progress update to set progress bar.
		static const QRegularExpression rx{"[^0-9]"};
		ui->progressBar->setValue(progressUpdates.last().remove(rx).toInt());

		// clean-out progress updates from converter output
		converterOutput.remove(progressRx);
	}

	qDebug() << converterOutput;
	static const QRegularExpression rxNewline{"\\r?\\n"};
	converterOutput.replace(rxNewline, QStringLiteral("<br/>"));

	if(!converterOutput.isEmpty()) {
		ui->ConverterOutputText->append(QStringLiteral("<font color=\"%1\"> %2 </font>")
										.arg(channel == 2 ? consoleLtGreen : consoleGreen, converterOutput)
										);
		ui->ConverterOutputText->verticalScrollBar()->triggerAction(QScrollBar::SliderToMaximum);
	}
}

void MainWindow::on_ConverterStarted()
{
	ui->convertButton->setIsActive(true);
	ui->progressBar->setValue(0);
	if(bShowProgressBar) {
		ui->progressBar->setVisible(true);
	}
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

void MainWindow::onBrowseInButtonRightClicked()
{
	for (auto& a : browseInMenu->actions()) {
		browseInMenu->removeAction(a);
	}

	browseInMenu->addAction("Select Files ...", this, &MainWindow::on_browseInfileButton_clicked);
	browseInMenu->addAction("Select Entire Directory ...", this, [this] {
		QFileInfo fi(inFileBrowsePath);
		QString inFileBrowseDir;
		if(fi.isDir()) {
			inFileBrowseDir = inFileBrowsePath;
		} else {
			inFileBrowseDir = fi.path();
		}

		QFileDialog fileDialog(this);
		fileDialog.setWindowTitle(tr("Select Input Directory"));
		fileDialog.setDirectory(inFileBrowseDir);
		fileDialog.setFileMode(QFileDialog::Directory);
		fileDialog.setViewMode(QFileDialog::Detail);

		if(fileDialog.exec()) {
			QString fName = QDir::toNativeSeparators(fileDialog.selectedFiles().constFirst() + "/*");
			if(!fName.isEmpty())
				processInputFilenames(QStringList{fName});
		}
	});

	browseInMenu->popup(QCursor::pos());
}

void MainWindow::onBrowseOutButtonRightClicked()
{
	for (auto& a : browseOutMenu->actions()) {
		browseOutMenu->removeAction(a);
	}

	browseOutMenu->addAction("Select Output File ...", this, &MainWindow::on_browseOutfileButton_clicked);
	browseOutMenu->addAction("Select Output Directory ...", this, [this] {
		openChooseOutputDirectory();
	});
	browseOutMenu->addAction("Set Output File Options ...", this, &MainWindow::on_actionOutput_File_Options_triggered);
	browseOutMenu->popup(QCursor::pos());
}

void MainWindow::on_browseInfileButton_clicked()
{   
	QFileDialog fileDialog(this);

	QFileInfo fi(inFileBrowsePath);
	QString inFileBrowseDir;
	if(fi.isDir()) {
		inFileBrowseDir = inFileBrowsePath;
	} else {
		inFileBrowseDir = fi.path();
	}

	fileDialog.setDirectory(inFileBrowseDir);
	fileDialog.setFileMode(QFileDialog::ExistingFiles);
	fileDialog.setNameFilter(getInfileFilter());
	fileDialog.setViewMode(QFileDialog::Detail);
	QStringList fileNames;
	if(fileDialog.exec()) {
		fileNames = fileDialog.selectedFiles();
	} else {
		return;
	}

	processInputFilenames(fileNames);
}

void MainWindow::processInputFilenames(const QStringList& fileNames)
{
	QString filenameSpec;

	if(fileNames.isEmpty()) {
		return;
	}

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
				if(!outFileName.isNull() && !outFileName.isEmpty()) {
					ui->OutfileEdit->setText(outFileName);
					ui->OutfileEdit->update();
				}
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
			filenameSpec += multiFileSeparator;
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
	processOutfileExtension();
}

void MainWindow::on_convertButton_clicked()
{
	launchType = LaunchType::Convert;
	launch();
}

void MainWindow::launch()
{
	QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
	ui->StatusLabel->setText("Status: Preparing conversion tasks ...");
	repaint();
	QStringList filenames=ui->InfileEdit->text().split(multiFileSeparator);

	// iterate over the filenames, adding either a single conversion, or wildcard conversion at each iteration
	for (const QString& inFilename : qAsConst(filenames)) {

		if(!inFilename.isEmpty() && !inFilename.isNull()) {

			// Search for Wildcards:
			if(inFilename.lastIndexOf("*") > -1) { // Input Filename has wildcard
				MainWindow::wildcardPushToQueue(inFilename);
			}

			else { // No Wildcard:
				ConversionTask T;
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

	ui->StatusLabel->setText("Status: Ready");
	QApplication::restoreOverrideCursor();
	MainWindow::convertNext();
}

// wildcardPushtoQueue() - expand wildcard in filespec, and push matching filenames into queue:
void MainWindow::wildcardPushToQueue(const QString& inFilename)
{
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
	FilenameGenerator f(MainWindow::filenameGenerator); // initialize to default settings, as a fallback position.

	// initialize output directory:
	f.outputDirectory = QDir::toNativeSeparators(outDir);
	f.useSpecificOutputDirectory = true;

	// initialize output file extension:
	int outLastDot = ui->OutfileEdit->text().lastIndexOf(".");
	if(outLastDot > -1) {
		f.fileExt = ui->OutfileEdit->text().right(ui->OutfileEdit->text().length() - outLastDot - 1); // get file extension from file nam
		if(f.fileExt.lastIndexOf("*") > -1) { // outfile extension has a wildcard in it
			f.useSpecificFileExt = false;   // use source file extension
		} else {
			f.useSpecificFileExt = true;    // use file extension of outfile name
		}
	} else { // outfile name has no file extension
		f.useSpecificFileExt = false; // use source file extension
	}

	// initialize output file suffix:
	// (use whatever is between last '*' and '.')
	int outLastStarBeforeDot = ui->OutfileEdit->text().leftRef(outLastDot).lastIndexOf("*");
	if(outLastStarBeforeDot > -1) {
		f.suffix = ui->OutfileEdit->text().mid(outLastStarBeforeDot + 1, outLastDot-outLastStarBeforeDot - 1); // get what is between last '*' and last '.'
		f.appendSuffix = true;
	} else { // no Suffix
		f.suffix = "";
		f.appendSuffix = false;
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

		if (!match.hasMatch()) {
			continue; // no match ? move on to next file ...
		}

		ConversionTask conversionTask;
		conversionTask.inFilename = QDir::toNativeSeparators(nextFilename);

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
			QDir dir(QDir::toNativeSeparators(f.outputDirectory + "/" + sd));
			QString p(dir.absolutePath());

			bool newDirCreated = false;

			if(launchType == LaunchType::Clipboard) {
				if(!createdDirectoriesList.contains(p)) {
					// get current clipboard text and append new line to it:
					QString clipText = QGuiApplication::clipboard()->text();
					QTextStream out(&clipText);
					out << QString{"mkdir \"%1\"\n"}.arg(QDir::toNativeSeparators(p));
					QGuiApplication::clipboard()->setText(clipText);
					createdDirectoriesList.append(p);
				}
			}

			else if(ui->actionMock_Conversion->isChecked()) { // mock-create directory
				if(!createdDirectoriesList.contains(p)) {
					newDirCreated = true;
					createdDirectoriesList.append(p);
				}
			}

			else { // create directory for real
				if (!dir.exists()) {
					newDirCreated = true;
					dir.mkpath(".");
				}
			}

			if(newDirCreated) {
				ui->ConverterOutputText->append(QString{"<font color=\"%1\"> mkdir %2 </font>"}
												.arg(consoleYellow, QDir::toNativeSeparators(p)));
			}
		}

		f.generateOutputFilename(conversionTask.outFilename, conversionTask.inFilename, QDir::toNativeSeparators(sd));
#else
		f.generateOutputFilename(conversionTask.outFilename, conversionTask.inFilenam);
#endif
		MainWindow::conversionQueue.push_back(conversionTask);
	}
}

// convertNext() - take the next conversion task from the front of the queue, convert it, then remove it from queue.

void MainWindow::convertNext()
{
	if(!conversionQueue.empty()) {
		ConversionTask& nextTask = MainWindow::conversionQueue.first();
		ui->StatusLabel->setText(tr("Status: processing ") + nextTask.inFilename);
		ui->progressBar->setFormat(tr("Status: processing ") + nextTask.inFilename);
		this->repaint();
		MainWindow::convert(nextTask.outFilename, nextTask.inFilename);
		conversionQueue.removeFirst();
	}
}

// convert() - the function that actually launches the converter(s)
// converts file infn to outfn using current parameters

void MainWindow::convert(const QString &outfn, const QString& infn)
{
	if(outfn.isEmpty() || infn.isEmpty()) {
		return;
	}

	// Up to three actual converters (ie conversion programs) may be employed to complete the conversion task.
	// ReSampler is the middle converter, and specialist converters may also be patched-in at the front or back positions

	// +-------------------+  +------------------- +  +-------------------+
	// |  front Converter  |  |  Mid Converter     |  |  back Converter   |
	// |                   |  |    (ReSsampler)    |  |                   |
	// | in            out |  | in             out |  | in            out |
	// +-------------------+  +--------------------+  +-------------------+

	QFileInfo inFileInfo(infn);
	QFileInfo outFileInfo(outfn);

	QString infn_ext(inFileInfo.suffix());
	QString outfn_ext(outFileInfo.suffix());

	QString frontConverterIn;
	QString frontConverterOut;
	QString midConverterIn;
	QString midConverterOut;
	QString backConverterIn;
	QString backConverterOut;

	QString frontCommandLine;
	QString midCommandLine;
	QString backCommandLine;

#ifdef Q_OS_WIN
	const QString delCommand{"del"};
#else
	const QString delCommand{"rm"};
#endif

	// is the input format to be handled by a specialist converter ?
	ConverterDefinition frontConverter = getSpecialistConverter(infn_ext, "wav");
	if(frontConverter.name.isEmpty()) {
		midConverterIn = infn;
	} else {
		frontConverterIn = infn;
		frontConverterOut = QDir::toNativeSeparators(QDir::tempPath() + "/" + getRandomString(8) + ".wav");
		midConverterIn = frontConverterOut;
		frontCommandLine = prepareSpecialistConverterArgs(frontConverter, frontConverterOut, frontConverterIn).join(" ");
	}

	// is the output format to be handled by a specialist converter ?
	ConverterDefinition backConverter = getSpecialistConverter("wav", outfn_ext);
	if(backConverter.name.isEmpty()) {
		midConverterOut = outfn;
	} else {
		midConverterOut = QDir::toNativeSeparators(QDir::tempPath() + "/" + getRandomString(8) + ".wav");
		backConverterIn = midConverterOut;
		backConverterOut = outfn;
		backCommandLine = prepareSpecialistConverterArgs(backConverter,  backConverterOut, backConverterIn).join(" ");
	}

	// prepare central conversion:
	midCommandLine = getQuotedArgs(prepareMidConverterArgs(midConverterOut, midConverterIn)).join(" ");

	QStringList combinedArgs;
	if(!frontCommandLine.isEmpty())
		combinedArgs << frontCommandLine;

	if(!midCommandLine.isEmpty()) {
		combinedArgs << midCommandLine;
		if(!frontConverterOut.isEmpty()) {
			combinedArgs << QString{"%1 %2"}.arg(delCommand, frontConverterOut);  // add command to delete temp file
		}
	}

	if(!backCommandLine.isEmpty()) {
		combinedArgs << backCommandLine;
		if(!midConverterOut.isEmpty()) {
			combinedArgs << QString{"%1 %2"}.arg(delCommand, midConverterOut);  // add command to delete temp file
		}
	}

	QString completeCmdLine = combinedArgs.join(QByteArray(" && "));

	if(launchType == LaunchType::Convert) {

		if(ui->actionMock_Conversion->isChecked()) {
			ui->ConverterOutputText->append(QString{"<font color=\"%1\"> mkdir %2 </font>"}.arg(consoleAmber, completeCmdLine));
			QTimer::singleShot(25, this, [this] {
				on_ConverterFinished(0, QProcess::NormalExit);
			});
		}
		else {
			process.setProcessChannelMode(QProcess::SeparateChannels);

#ifdef Q_OS_WIN
			process.start("cmd.exe /c " + completeCmdLine);
#else
			process.start("bash", QStringList() << "-c" << completeCmdLine);
#endif

		}
	}

	else if(launchType == LaunchType::Clipboard) {

		// get current clipboard text and append new line to it:
		QString clipText = QGuiApplication::clipboard()->text();
		QTextStream out(&clipText);
		out << completeCmdLine << "\n";
		QGuiApplication::clipboard()->setText(clipText);

		QTimer::singleShot(5, this, [this] {
			on_ConverterFinished(0, QProcess::NormalExit);
		});
	}
}

ConverterDefinition MainWindow::getSpecialistConverter(const QString& inExt, const QString& outExt )
{
	for(const ConverterDefinition& converterDefinition : qAsConst(converterDefinitions)) {
		if(converterDefinition.inputFileExt == inExt && converterDefinition.outputFileExt == outExt) {
			return converterDefinition;
		}
	}
	return ConverterDefinition();
}

// getQuotedArgs() : wrap args in quotes if necessary
// 1. spaces definitely need quotes
// 2. parentheses and backslashes, and probably a whole lot of other characters, can cause problems with bash

QStringList MainWindow::getQuotedArgs(const QStringList& args)
{
	QStringList quotedArgs;
	for(const QString& arg : args) {
		quotedArgs.append(arg.contains(QRegExp("[() \\\\]")) ? "\"" + arg + "\"" : arg);
	}
	return quotedArgs;
}


QStringList MainWindow::prepareSpecialistConverterArgs(const ConverterDefinition& converterDefinition, const QString& outfn, const QString& infn)
{
	QStringList args;
	QString params = converterDefinition.commandLine;
	params.replace("{i}", "\"" + infn + "\"").replace("{o}", "\"" + outfn + "\"");
	args << converterDefinition.executablePath << params;
	return args;
}

QStringList MainWindow::prepareMidConverterArgs(const QString &outfn, const QString& infn)
{

	QStringList args;

	// start with the program
	args << QDir::toNativeSeparators(converterPath);

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
	switch(lpfType) {
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

	// format args: progress update frequency
	args << "--progress-updates" << "100";

	return args;
}

void MainWindow::loadConverterDefinitions(const QString& fileName)
{
	converterDefinitions = ConverterDefinition::loadConverterDefinitions(fileName);
	checkConverterAvailability();
}

void MainWindow::saveConverterDefinitions(const QString& fileName) const
{
	ConverterDefinition::saveConverterDefinitions(fileName, converterDefinitions);
}

void MainWindow::on_InfileEdit_editingFinished()
{
	QString inFilename = ui->InfileEdit->text();

	if(inFilename.isEmpty()) {

		// reset to single file mode:
		ui->InfileLabel->setText(tr("Input File:"));
		ui->OutfileLabel->setText(tr("Output File:"));
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
			int InLastStarBeforeDot = inFilename.leftRef(InLastDot).lastIndexOf("*");
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

	if(inFilename.right(1) == multiFileSeparator) {
		inFilename=inFilename.left(inFilename.size() - 1); // Trim Multifile separator off the end
		ui->InfileEdit->setText(inFilename);
	}

	if(inFilename.indexOf(multiFileSeparator) == -1) { // Single-file mode:

		ui->InfileLabel->setText(tr("Input File:"));
		ui->OutfileLabel->setText(tr("Output File:"));
		ui->OutfileEdit->setReadOnly(false);

		if(bRefreshOutfileEdit) {
			filenameGenerator.generateOutputFilename(outFilename, ui->InfileEdit->text());
			if(!outFilename.isNull() && !outFilename.isEmpty())
				ui->OutfileEdit->setText(outFilename);
			ui->OutfileEdit->update();
		}
	}

	else { // multi-file mode:

		QString outFilename=inFilename.left(inFilename.indexOf(multiFileSeparator)); // use first filename as a basis for generating output filename
		int LastDot = outFilename.lastIndexOf(".");
		int LastSep = outFilename.lastIndexOf(QDir::separator());
		QString s = outFilename.mid(LastSep + 1, LastDot - LastSep - 1); // get what is between last separator and last '.'
		if(!s.isEmpty() && !s.isNull()) {
			outFilename.replace(s, "*"); // replace everything between last separator and file extension with a wildcard ('*'):
		}
		filenameGenerator.generateOutputFilename(outFilename, outFilename); // Generate output filename by applying name-generation rules
		ui->OutfileEdit->setText(outFilename);
		ui->OutfileLabel->setText(tr("Output Files: (filenames auto-generated)"));
		ui->OutfileEdit->setReadOnly(true);
		ui->OutfileEdit->update();
		ui->InfileLabel->setText(tr("Input Files:"));
	}

	// trigger an update of options if output file extension changed:
	processOutfileExtension();
}

void MainWindow::on_browseOutfileButton_clicked()
{
	QStringList infileList = ui->InfileEdit->text().split(multiFileSeparator);
	if(infileList.count() > 1) {
		// multi-file mode
		openChooseOutputDirectory();
		return;
	}

	QString path = ui->OutfileEdit->text().isEmpty() ? outFileBrowsePath : ui->OutfileEdit->text(); // if OutfileEdit is populated, use that. Otherwise, use last output file browse path
	QString fileName = QFileDialog::getSaveFileName(this, tr("Select Output File"), path, getOutfileFilter());

	if(!fileName.isNull()) {
		QDir path(fileName);
		outFileBrowsePath = path.absolutePath(); // remember this browse session (Unix separators)
		ui->OutfileEdit->setText(QDir::toNativeSeparators(fileName));
		populateBitFormats(ui->OutfileEdit->text());

		// trigger an update of options if file extension changed:
		processOutfileExtension();
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
void MainWindow::populateBitFormats(const QString& fileName)
{
	QProcess ConverterQuery;

	int extidx = fileName.lastIndexOf(".");
	const QString fallBackExtension{"wav"};

	// get file extension from file name, or use default
	QString ext = (extidx > -1) ? fileName.right(fileName.length() - extidx - 1) : fallBackExtension;

	bool bitFormatsAcquired;
	do {
		ui->BitDepthCombo->clear();
		ConverterQuery.start(converterPath, QStringList() << "--listsubformats" << ext); // ask converter for a list of subformats for the given file extension

		if(!ConverterQuery.waitForStarted(1000)) {
			return;
		}

		if (!ConverterQuery.waitForFinished(2000)) {
			return;
		}

		bitFormatsAcquired = true;
		ConverterQuery.setReadChannel(QProcess::StandardOutput);

		while(ConverterQuery.canReadLine()) {
			QString line = QString::fromLocal8Bit(ConverterQuery.readLine());
			if(line.contains("unknown")) {
				ext = fallBackExtension;
				bitFormatsAcquired = false;
				if(ui->BitDepthCombo->currentText().isEmpty()) {
					ui->BitDepthCombo->setCurrentText("16"); // default to 16-bit
				}
				break;
			}
			ui->BitDepthCombo->addItem(line.simplified());
		}
	} while (!bitFormatsAcquired);
}

// Query Converter for version number:
void MainWindow::queryResamplerVersion()
{
	const QString v = queryResampler({"--version"});

	// split the version number into components:
	QStringList ResamplerVersionNumbers = v.split(".");

	// set various options accoring to resampler version:
	int vA=ResamplerVersionNumbers[0].toInt(); // 1st number
	int vB=ResamplerVersionNumbers[1].toInt(); // 2nd number

	bShowProgressBar = (vA > 1) || (vB >= 1); // (no progress output on ReSampler versions prior to 1.1.0)
	resamplerVersion=v;
}

void MainWindow::queryResamplerSndfileVersion()
{
	static const QRegularExpression rx{".*(?:\\d+)\\.(\\d+)\\.(?:\\d+)"};

	resamplerSndfileVersion = queryResampler({"--sndfile-version"});
	auto rxm = rx.match(resamplerSndfileVersion);

	if(rxm.hasMatch()) {
		bReSamplerMp3 = (rxm.captured(1).toInt() >= 1);
	}
}

QString MainWindow::queryResampler(const QStringList& cmdlineOptions)
{
	QString v;
	QProcess ConverterQuery;
	ConverterQuery.start(converterPath, cmdlineOptions); // ask converter for its version number

	if (!ConverterQuery.waitForFinished()) {
		return {};
	}

	ConverterQuery.setReadChannel(QProcess::StandardOutput);
	while(ConverterQuery.canReadLine()) {
		v += (QString::fromLocal8Bit(ConverterQuery.readLine())).simplified();
	}

	return v;
}

void MainWindow::on_OutfileEdit_editingFinished()
{
	processOutfileExtension(); // trigger an update of options if user changed the file extension
}

// ProcessoutFileExtension() - analyze extension of outfile and update subformats dropdown accordingly
void MainWindow::processOutfileExtension()
{

	QString fileName = ui->OutfileEdit->text();
	int extidx = fileName.lastIndexOf(".");
	if(extidx > -1) { // filename must have a "." to contain a file extension ...
		QString ext = fileName.right(fileName.length() - extidx - 1); // get file extension from file name

		// if user has changed the extension (ie type) of the filename, then repopulate subformats combobox:
		if(ext != lastOutputFileExt) {
			populateBitFormats(fileName);
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
	converterConfigurationDialog->setExpectedMainConverter(expectedConverter);
	converterConfigurationDialog->setMainConverterPath(converterPath);
	converterConfigurationDialog->setConverterDefinitions(converterDefinitions);
	converterConfigurationDialog->setMinimumWidth(this->width());
	converterConfigurationDialog->setShowToolTips(ui->actionEnable_Tooltips->isChecked());
	if(converterConfigurationDialog->exec() == QDialog::Accepted) {

		if(!converterConfigurationDialog->getMainConverterPath().isEmpty()) {
			converterPath = converterConfigurationDialog->getMainConverterPath();
			queryResamplerVersion();  // get converter version
		}
		converterDefinitions = converterConfigurationDialog->getConverterDefinitions();
		checkConverterAvailability();
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
	QString info(tr("Ferocious File Conversion\n By J.Niemann\n\n"));

	info += tr("GUI Version: ") + QString(APP_VERSION) + "\n";
	info += tr("Converter Version: ") + resamplerVersion + "\n";

	QMessageBox msgBox;
	msgBox.setText(tr("About"));
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
	if (event->type() == QEvent::ToolTip) { // Intercept tooltip event
		return (!ui->actionEnable_Tooltips->isChecked());
	}

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

void MainWindow::applyStylesheet()
{

	if(stylesheetFilePath.isEmpty())
		stylesheetFilePath = ":/ferocious.css"; // factory default

	if(stylesheetFilePath == ":/ferocious.css") {
		qDebug() << tr("using factory default theme");
		return;
	}

	if(!fileExists(stylesheetFilePath)) {
		qDebug() << tr("stylesheet ") << stylesheetFilePath << tr(" doesn't exist");
		return;
	}

	QApplication* a = qApp;

	// retrieve and apply Stylesheet:
	QFile ss(stylesheetFilePath);
	if(ss.open(QIODevice::ReadOnly | QIODevice::Text)) {
		a->setStyleSheet(ss.readAll());
		ss.close();
	} else {
		qDebug() << tr("Couldn't open stylesheet resource ") << stylesheetFilePath;
	}
}

void MainWindow::on_actionTheme_triggered()
{
	stylesheetFilePath = QFileDialog::getOpenFileName(this, tr("Choose a Stylesheet"), QDir::currentPath(), tr("Style Sheets (*.qss *.css)"));
	applyStylesheet();
}

void MainWindow::on_actionRelaxedLPF_triggered()
{
	lpfType = relaxedLPF;
	ui->actionRelaxedLPF->setChecked(true);
	ui->actionStandardLPF->setChecked(false);
	ui->actionSteepLPF->setChecked(false);
	ui->actionCustomLPF->setChecked(false);
	ui->actionCustom_Parameters->setVisible(false);
}

void MainWindow::on_actionStandardLPF_triggered()
{
	lpfType = standardLPF;
	ui->actionRelaxedLPF->setChecked(false);
	ui->actionStandardLPF->setChecked(true);
	ui->actionSteepLPF->setChecked(false);
	ui->actionCustomLPF->setChecked(false);
	ui->actionCustom_Parameters->setVisible(false);
}

void MainWindow::on_actionSteepLPF_triggered()
{
	lpfType = steepLPF;
	ui->actionRelaxedLPF->setChecked(false);
	ui->actionStandardLPF->setChecked(false);
	ui->actionSteepLPF->setChecked(true);
	ui->actionCustomLPF->setChecked(false);
	ui->actionCustom_Parameters->setVisible(false);
}

void MainWindow::on_actionCustomLPF_triggered()
{
	lpfType = customLPF;
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
	ConverterQuery.start(converterPath, QStringList() << "--showDitherProfiles");
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

void MainWindow::getCustomLpfParameters()
{
	auto d = new LpfParametersDlg(this);
	d->setValues(customLpfCutoff, customLpfTransition);
	d->setWindowTitle(tr("Custom LPF Parameters"));
	connect(d, &QDialog::accepted, this, [this, d] {
		auto v = d->getValues();
		customLpfCutoff = v.first;
		customLpfTransition = v.second;
	});
	d->exec();
}

void MainWindow::onConvertButtonRightClicked()
{
	for (auto& a : convertTaskMenu->actions()) {
		convertTaskMenu->removeAction(a);
	}

	convertTaskMenu->addAction(tr("Convert"), this, &MainWindow::on_convertButton_clicked);
	convertTaskMenu->addAction(tr("Copy Command Line to Clipboard"), [this] {
		QGuiApplication::clipboard()->clear();
		launchType = LaunchType::Clipboard;
		launch();
	});

	convertTaskMenu->popup(QCursor::pos());
}

void MainWindow::on_stopRequested()
{
	conversionQueue.clear();
	process.kill();
	ui->StatusLabel->setText(tr("Status: conversion stopped"));
}

void MainWindow::on_actionMultiStageConversion_triggered(bool checked)
{
	MainWindow::bSingleStage = !checked;
}

void MainWindow::on_actionUse_a_temp_file_triggered(bool checked)
{
	bNoTempFile = !checked;
}

// getInfileFiler() : returns a filename filter, taking into consideration all file formats which can be handled in the current state
QString MainWindow::getInfileFilter()
{
	QSet<QString> infileFormats{
		"*.aif",
		"*.aifc",
		"*.aiff",
		"*.au",
		"*.avr",
		"*.caf",
		"*.dff",
		"*.dsf",
		"*.flac",
		"*.htk",
		"*.iff",
		"*.mat",
		"*.mpc",
		"*.oga",
		"*.paf",
		"*.pvf",
		"*.raw",
		"*.rf64",
		"*.sd2",
		"*.sds",
		"*.sf",
		"*.voc",
		"*.w64",
		"*.wav",
		"*.wve",
		"*.xi"
	};

	if(bReSamplerMp3) {
		infileFormats.insert("*.m1a");
		infileFormats.insert("*.mp2");
		infileFormats.insert("*.mp3");
	}

	for(const ConverterDefinition& d : qAsConst(converterDefinitions)) {
		if(d.enabled) {
			infileFormats.insert(QString{"*."} + d.inputFileExt);
		}
	}

	return QString{"Audio Files (%1)"}.arg(infileFormats.values().join(" "));
}

QString MainWindow::getOutfileFilter()
{
	QSet<QString> outfileFormats{
		"*.aiff",
		"*.au",
		"*.avr",
		"*.caf",
		"*.flac",
		"*.htk",
		"*.iff",
		"*.mat",
		"*.mpc",
		"*.oga",
		"*.paf",
		"*.pvf",
		"*.raw",
		"*.rf64",
		"*.sd2",
		"*.sds",
		"*.sf",
		"*.voc",
		"*.w64",
		"*.wav",
		"*.wve",
		"*.xi"
	};

	if(bReSamplerMp3) {
		outfileFormats.insert("*.mp3");
	}

	for(const ConverterDefinition& d : qAsConst(converterDefinitions)) {
		if(d.enabled) {
			outfileFormats.insert(QString{"*."} + d.outputFileExt);
		}
	}

	return QString{"Audio Files (%1)"}.arg(outfileFormats.values().join(" "));
}

// check if each executable exists and disable if not found
void MainWindow::checkConverterAvailability()
{
	for(ConverterDefinition& d : converterDefinitions) {
		d.enabled = d.enabled && !d.executablePath.isEmpty() && QFile::exists(d.executablePath);
	}
}

QString MainWindow::getRandomString(int length)
{
	static QRandomGenerator rng{static_cast<quint32>(QDateTime::currentSecsSinceEpoch() & 0xffffffff)};

	QString s;
	const QString charSet("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
	for(int i = 0; i < length; ++i) {
		s.append(charSet.at(rng.bounded(0, static_cast<int>(charSet.size()) - 1)));
	}
	return s;
}

void MainWindow::openChooseOutputDirectory()
{
	QFileInfo fi(outFileBrowsePath);
	QString outFileBrowseDir;
	if(fi.isDir()) {
		outFileBrowseDir = outFileBrowsePath;
	} else {
		outFileBrowseDir = fi.path();
	}

	QFileDialog fileDialog(this);
	fileDialog.setWindowTitle(tr("Select Output Directory"));
	fileDialog.setDirectory(outFileBrowseDir);
	fileDialog.setFileMode(QFileDialog::Directory);
	fileDialog.setViewMode(QFileDialog::Detail);

	if(fileDialog.exec()) {
		QString dirName = QDir::toNativeSeparators(fileDialog.selectedFiles().first());
		if(!dirName.isEmpty()) {
			filenameGenerator.outputDirectory = dirName;
			outFileBrowsePath = dirName;
			MainWindow::on_InfileEdit_editingFinished(); // trigger refresh
		}
	}
}
