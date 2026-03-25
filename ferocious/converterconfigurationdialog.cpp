/*
* Copyright (C) 2016 - 2026 Judd Niemann - All Rights Reserved.
* You may use, distribute and modify this code under the
* terms of the GNU Lesser General Public License, version 2.1
*
* You should have received a copy of GNU Lesser General Public License v2.1
* with this file. If not, please refer to: https://github.com/jniemann66/ferocious
*/

#include "converterconfigurationdialog.h"
#include "converterconfigurationeditdialog.h"
#include "checkboxdelegate.h"
#include "cmdlinehighlighterdelegate.h"

#include <QApplication>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

ConverterConfigurationDialog::ConverterConfigurationDialog(QWidget* parent, Qt::WindowFlags f)
    : QDialog(parent, f), showToolTips(true)
{
    // allocate
    auto headingLabel = new QLabel("Configure External Converters");
    mainConverterLocationLabel = new QLabel("Location of Main Converter:");
    mainConverterLocationEdit = new FancyLineEdit;
    contextMenu = new QMenu(this);
    browseButton = new QPushButton("Browse ...");
    auto tableHintLabel = new QLabel(tr("Right-click a row for options  \u2022  Double-click to edit"));
    additionalConvertersLabel = new QLabel("Additional converters:");
    auto stdButtons = new QDialogButtonBox(QDialogButtonBox::RestoreDefaults | QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    auto mainLayout = new QVBoxLayout;
    auto mainConverterLayout = new QHBoxLayout;

    // set tooltips
    additionalConvertersLabel->setToolTip("Use the table below to cofigure additional converters for specialized file formats.");
    QPushButton* restoreDefaultsButton = stdButtons->button(QDialogButtonBox::RestoreDefaults);
    if (restoreDefaultsButton != nullptr) {
        restoreDefaultsButton->setToolTip(tr("Restore converter configuration to default settings"));
    }

    // set model
    tableView.setModel(&convertersModel);

    // configure view
    tableView.verticalHeader()->setHidden(true);
    tableView.setSelectionMode(QAbstractItemView::SingleSelection);
    tableView.setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView.setContextMenuPolicy(Qt::CustomContextMenu);
    tableView.horizontalHeader()->setStretchLastSection(true);

    // configure menu
    initMenu();

    // configure fonts
    QFont defaultFont{qApp->font()};
    QFont heading2Font{defaultFont};
    QFont heading1Font{defaultFont};
    QFont hintFont{defaultFont};
    hintFont.setPointSize(defaultFont.pointSize() - 1);

    // configure widgets
    headingLabel->setFont(heading1Font);
    headingLabel->setAlignment(Qt::AlignHCenter);
    mainConverterLocationEdit->hideEditButton();
    mainConverterLocationLabel->setFont(heading2Font);
    additionalConvertersLabel->setFont(heading2Font);
    tableHintLabel->setFont(hintFont);
    tableHintLabel->setAlignment(Qt::AlignRight);
    tableHintLabel->setForegroundRole(QPalette::Mid);

    // attach widgets to main layout
    mainLayout->addWidget(headingLabel);
    mainLayout->addSpacing(6);
    mainLayout->addWidget(mainConverterLocationLabel);
    mainConverterLayout->addWidget(mainConverterLocationEdit);
    mainConverterLayout->addWidget(browseButton);
    mainLayout->addLayout(mainConverterLayout);
    mainLayout->addSpacing(6);
    mainLayout->addWidget(additionalConvertersLabel);
    mainLayout->addWidget(&tableView);
    mainLayout->addWidget(tableHintLabel);
    mainLayout->addWidget(stdButtons);
    setLayout(mainLayout);

    // hide unnecessary columns
    tableView.setColumnHidden(0, true);
    tableView.setColumnHidden(3, true);
    tableView.setColumnHidden(6, true);
    tableView.setColumnHidden(9, true);
    tableView.setColumnHidden(10, true);

    // set delegates
    tableView.setItemDelegateForColumn(8, new CmdLineHighlighterDelegate(this));
    tableView.setItemDelegateForColumn(1, new CheckBoxDelegate(this));

    // connect signals / slots
    connect(mainConverterLocationEdit, &QLineEdit::editingFinished, this, [this] {
       mainConverterPath = mainConverterLocationEdit->text();
    });

    connect(browseButton, &QPushButton::clicked, this, &ConverterConfigurationDialog::promptForResamplerLocation);
    connect(&tableView, &QTableView::doubleClicked, this, [this](const QModelIndex& modelIndex) {
        onEditRequested(modelIndex);
    });

    connect(&tableView, &QWidget::customContextMenuRequested, this, [this](const QPoint& pos){
        contextMenu->popup(QPoint{this->mapToGlobal(pos).x(), this->mapToGlobal(pos).y() + contextMenu->sizeHint().height()});
    });

    connect(stdButtons, &QDialogButtonBox::clicked, this, [this, stdButtons](QAbstractButton* b) {
        if (b == stdButtons->button(QDialogButtonBox::RestoreDefaults)) {
            onRestoreDefaults();
        }
    });

    connect(stdButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(stdButtons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void ConverterConfigurationDialog::initMenu()
{
	contextMenu->addAction(tr("New"), QKeySequence::New, this, [this] {
       onNewRequested(tableView.currentIndex());
    });

	contextMenu->addAction(tr("Edit ..."), this, [this] {
       onEditRequested(tableView.currentIndex());
    });

	contextMenu->addAction(tr("Clone"), QKeySequence::Copy, this, [this] {
       onCloneRequested(tableView.currentIndex());
    });

	contextMenu->addAction(tr("Delete"), QKeySequence::Delete, this, [this] {
       onDeleteRequested(tableView.currentIndex());
    });

	contextMenu->addAction(tr("Move Up"), this, [this] {
        onMoveUpRequested(tableView.currentIndex());
    });

	contextMenu->addAction(tr("Move Down"), this, [this] {
        onMoveDownRequested(tableView.currentIndex());
    });
}

void ConverterConfigurationDialog::showEvent(QShowEvent* event)
{

	if (mainConverterPath.isEmpty() || !QFile::exists(mainConverterPath)) {
#if defined(Q_OS_WIN)
        const QString defaultPath = QString("C:/Program Files/ReSampler/bin/%1").arg(expectedMainConverter);
        if (QFile::exists(defaultPath)) {
            mainConverterPath = defaultPath;
        } else {
            promptForResamplerLocation();
        }
#else
        promptForResamplerLocation();
#endif
	}

    mainConverterLocationLabel->setText(QString{"Location of Main Converter (%1):"}.arg(expectedMainConverter));
    mainConverterLocationEdit->setText(mainConverterPath);

    QDialog::showEvent(event);
}

void ConverterConfigurationDialog::resizeEvent(QResizeEvent *event)
{
    const int tw = event->size().width();

    static const QVector<double> columnWidths {
        0,      /* "Priority" */
        10,     /* "Enabled" */
        20,     /* "Name" */
        0 ,     /* "Comment" */
        12,     /* "Input File Extension" */
        12,     /* "Output File Extension" */
        0 ,     /* "Executable" */
        20,     /* "Executable Path" */
        20,     /* "Command Line" */
        0 ,     /* "Download Locations" */
        0      /* "Operating Systems" */
    };

    for (int col = 0; col < convertersModel.columnCount({}) - 1; col++) {
        tableView.horizontalHeader()->resizeSection(col, tw * columnWidths.at(col)/100.0);
    }

    tableView.horizontalHeader()->setHidden(false);

    QDialog::resizeEvent(event);
}

QVector<ConverterDefinition> ConverterConfigurationDialog::getConverterDefinitions() const
{
    return convertersModel.getConverterDefinitions();
}

void ConverterConfigurationDialog::setConverterDefinitions(const QVector<ConverterDefinition> &value)
{
    convertersModel.setConverterDefinitions(value);
}

void ConverterConfigurationDialog::promptForResamplerLocation() {

	QString msg = tr("Ferocious is designed to work in conjunction with ReSampler<br/>Please show ferocious where the ReSampler executable is located");
	QMessageBox::information(this, tr("Please locate the ReSampler binary"), msg);

    QString s(tr("Please locate the file: "));
    s.append(expectedMainConverter);

#if defined (Q_OS_WIN)
    QString filter = "*.exe";
#else
    QString filter = "";
#endif

    QFileDialog dialog(this);
    dialog.setWindowTitle(s);
    dialog.setDirectory(mainConverterPath);
    dialog.setNameFilter(filter);
    if (dialog.exec()) {
        const QString cp = dialog.selectedFiles().first();
        mainConverterPath = cp;
        if (mainConverterPath.lastIndexOf(expectedMainConverter, -1, Qt::CaseInsensitive) == -1) { // safeguard against wrong executable being configured
            mainConverterPath.clear();
            QMessageBox::warning(this, tr("Converter Location"), tr("That is not the right program!\n"), QMessageBox::Ok);
        }
    }
}

QString ConverterConfigurationDialog::getMainConverterPath() const
{
    return mainConverterPath;
}

void ConverterConfigurationDialog::setMainConverterPath(const QString &value)
{
    mainConverterPath = value;
}

QString ConverterConfigurationDialog::getExpectedMainConverter() const
{
    return expectedMainConverter;
}

void ConverterConfigurationDialog::setExpectedMainConverter(const QString &value)
{
    expectedMainConverter = value;
    mainConverterLocationLabel->setToolTip(QString{"Please enter the location of %1 in the box below.\n"
                                           "(%1 is the command-line audio converter that Ferocious was designed to work with.)"
                                           }.arg(expectedMainConverter));
    mainConverterLocationEdit->setToolTip(QString{"Location of %1\n"
                                          "Note: you can also use drag-and-drop, or the 'Browse' button"}.arg(expectedMainConverter));
    browseButton->setToolTip(QString{"Browse to location of %1"}.arg(expectedMainConverter));
}

void ConverterConfigurationDialog::onNewRequested(const QModelIndex& modelIndex)
{
    int row = modelIndex.row();

    if (row < 0) {
        return;
    }

    QVector<ConverterDefinition> converterDefinitions = convertersModel.getConverterDefinitions();
    if (row < converterDefinitions.count()) {
        converterDefinitions.insert(row, {});
        convertersModel.setConverterDefinitions(converterDefinitions);
    }
}

void ConverterConfigurationDialog::onEditRequested(const QModelIndex& modelIndex)
{
    QVector<ConverterDefinition> converterDefinitions = convertersModel.getConverterDefinitions();
    if (converterDefinitions.isEmpty()) {
        return;
    }

    int row = modelIndex.row();

    if (row < 0) {
        return;
    }

    auto dlg = new ConverterConfigurationEditDialog(this);
    dlg->setShowToolTips(showToolTips);
    if (!editDialogGeometry.isEmpty()) {
        dlg->restoreGeometry(editDialogGeometry);
    }

    if (row < converterDefinitions.count()) {
        dlg->setConverterDefinition(converterDefinitions.at(row));
        int result = dlg->exec();
        editDialogGeometry = dlg->saveGeometry();
        if (result == QDialog::Accepted) {
            converterDefinitions[row] = dlg->getConverterDefinition();
            convertersModel.setConverterDefinitions(converterDefinitions);
        }
    }
}

void ConverterConfigurationDialog::onDeleteRequested(const QModelIndex& modelIndex)
{
    int row = modelIndex.row();

    if (row < 0) {
        return;
    }

    QVector<ConverterDefinition> converterDefinitions = convertersModel.getConverterDefinitions();
    if (row < converterDefinitions.count()) {
        converterDefinitions.removeAt(row);
        convertersModel.setConverterDefinitions(converterDefinitions);
    }
}

void ConverterConfigurationDialog::onCloneRequested(const QModelIndex& modelIndex)
{
    int row = modelIndex.row();

    if (row < 0) {
        return;
    }

    QVector<ConverterDefinition> converterDefinitions = convertersModel.getConverterDefinitions();
    if (row < converterDefinitions.count()) {
        converterDefinitions.insert(row, converterDefinitions.at(row));
        convertersModel.setConverterDefinitions(converterDefinitions);
    }
}

void ConverterConfigurationDialog::onMoveUpRequested(const QModelIndex& modelIndex)
{
    int row = modelIndex.row();

    if (row < 1) {
        return;
    }

    QVector<ConverterDefinition> converterDefinitions = convertersModel.getConverterDefinitions();
    if (row < converterDefinitions.count()) {
        qSwap(converterDefinitions[row], converterDefinitions[row - 1]);
        convertersModel.setConverterDefinitions(converterDefinitions);
        tableView.selectRow(row - 1);
    }
}

void ConverterConfigurationDialog::onMoveDownRequested(const QModelIndex& modelIndex)
{
    int row = modelIndex.row();

    if (row < 0) {
        return;
    }

    QVector<ConverterDefinition> converterDefinitions = convertersModel.getConverterDefinitions();
    if (row < converterDefinitions.count() - 1) {
        qSwap(converterDefinitions[row], converterDefinitions[row + 1]);
        convertersModel.setConverterDefinitions(converterDefinitions);
        tableView.selectRow(row + 1);
    }
}

void ConverterConfigurationDialog::onRestoreDefaults()
{
    setConverterDefinitions(ConverterDefinition::loadConverterDefinitions(":/converters.json"));
}

QByteArray ConverterConfigurationDialog::getEditDialogGeometry() const
{
    return editDialogGeometry;
}

void ConverterConfigurationDialog::setEditDialogGeometry(const QByteArray &value)
{
    editDialogGeometry = value;
}

void ConverterConfigurationDialog::setShowToolTips(bool value)
{
    showToolTips = value;
}

bool ConverterConfigurationDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::ToolTip) {
        return (showToolTips);
    }

	return ConverterConfigurationDialog::eventFilter(obj, event);
}
