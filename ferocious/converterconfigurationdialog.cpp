#include "converterconfigurationdialog.h"
#include "checkboxdelegate.h"
#include "cmdlinehighlighterdelegate.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QDebug>
#include <QApplication>

ConverterConfigurationDialog::ConverterConfigurationDialog(QWidget* parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    // allocate
    auto headingLabel = new QLabel("Configure External Converters");
    mainConverterLocationLabel = new QLabel("Location of Main Converter:");
    mainConverterLocationEdit = new FancyLineEdit;
    contextMenu = new QMenu(this);
    browseButton = new QPushButton("Browse ...");
    additionalConvertersLabel = new QLabel("Additional converters:");
    auto stdButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    auto mainLayout = new QVBoxLayout;
    auto mainConverterLayout = new QHBoxLayout;

    // set model
    tableView.setModel(&convertersModel);

    // configure view
    tableView.verticalHeader()->setHidden(true);
    tableView.setSelectionMode(QAbstractItemView::SingleSelection);
    tableView.setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView.setContextMenuPolicy(Qt::CustomContextMenu);


    // configure menu
    initMenu();

    // configure fonts
    QFont defaultFont{qApp->font()};
    QFont heading2Font{defaultFont};
    QFont heading1Font{defaultFont};
    heading2Font.setPointSize(defaultFont.pointSize() + 2);
    heading1Font.setPointSize(defaultFont.pointSize() + 4);

    // configure widgets
    headingLabel->setFont(heading1Font);
    headingLabel->setAlignment(Qt::AlignHCenter);
    mainConverterLocationEdit->hideEditButton();
    mainConverterLocationLabel->setFont(heading2Font);
    additionalConvertersLabel->setFont(heading2Font);

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
    connect(&tableView, &QWidget::customContextMenuRequested, this, [this](const QPoint& pos){
        contextMenu->popup(QPoint{this->mapToGlobal(pos).x(), this->mapToGlobal(pos).y() + contextMenu->sizeHint().height()});
    });
    connect(stdButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(stdButtons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void ConverterConfigurationDialog::initMenu() {
    contextMenu->addAction("New", [this] {
       onNewRequested(tableView.currentIndex());
    }, QKeySequence::New);

    contextMenu->addAction("Edit ...", [this] {
       onEditRequested(tableView.currentIndex());
    });

    contextMenu->addAction("Clone", [this] {
       onCloneRequested(tableView.currentIndex());
    }, QKeySequence::Copy);

    contextMenu->addAction("Delete", [this] {
       onDeleteRequested(tableView.currentIndex());
    }, {QKeySequence::Delete});

    contextMenu->addAction("Move Up", [this] {
        onMoveUpRequested(tableView.currentIndex());
    });

    contextMenu->addAction("Move Down", [this] {
        onMoveDownRequested(tableView.currentIndex());
    });

}

void ConverterConfigurationDialog::showEvent(QShowEvent* event) {

    if(mainConverterPath.isEmpty() || !QFile::exists(mainConverterPath)) {
       promptForResamplerLocation();
    }

    mainConverterLocationLabel->setText(QString{"Location of Main Converter (%1):"}.arg(expectedMainConverter));
    mainConverterLocationEdit->setText(mainConverterPath);

    QDialog::showEvent(event);
}

void ConverterConfigurationDialog::resizeEvent(QResizeEvent *event)
{
    int tw = event->size().width();

    static const QVector<double> columnWidths {
        0,      /* "Priority" */
        10,     /* "Enabled" */
        20,     /* "Name" */
        0 ,     /* "Comment" */
        15,     /* "Input File Extension" */
        15,     /* "Output File Extension" */
        0 ,     /* "Executable" */
        20,     /* "Executable Path" */
        20,     /* "Command Line" */
        0 ,     /* "Download Locations" */
        0      /* "Operating Systems" */
    };

    for(int col = 0; col < convertersModel.columnCount({}); col++) {
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
    QString s("Please locate the file: ");
    s.append(expectedMainConverter);

#if defined (Q_OS_WIN)
    QString filter = "*.exe";
#else
    QString filter = "";
#endif

    QString cp = QFileDialog::getOpenFileName(this, s, mainConverterPath,  filter);

    if(!cp.isNull()) {
        mainConverterPath = cp;
        if(mainConverterPath.lastIndexOf(expectedMainConverter, -1, Qt::CaseInsensitive) == -1) { // safeguard against wrong executable being configured
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
    // set tooltips
    mainConverterLocationLabel->setToolTip(QString{"Please enter the location of %1 in the box below.\n"
                                           "(%1 is the command-line audio converter that Ferocious was designed to work with.)"
                                           }.arg(expectedMainConverter));
    mainConverterLocationEdit->setToolTip(QString{"Location of %1\n"
                                          "Note: you can also use drag-and-drop, or the 'Browse' button"}.arg(expectedMainConverter));
    browseButton->setToolTip(QString{"Browse to location of %1"}.arg(expectedMainConverter));
    additionalConvertersLabel->setToolTip("Use the table below to cofigure additional converters for specialized file formats.");

}

void ConverterConfigurationDialog::onNewRequested(const QModelIndex& modelIndex)
{
    int row = modelIndex.row();

    if(row < 0 )
        return;

    QVector<ConverterDefinition> converterDefinitions = convertersModel.getConverterDefinitions();
    if(row < converterDefinitions.count()) {
        converterDefinitions.insert(row, {});
        convertersModel.setConverterDefinitions(converterDefinitions);
    }
}


void ConverterConfigurationDialog::onEditRequested(const QModelIndex& modelIndex)
{
    QVector<ConverterDefinition> converterDefinitions = convertersModel.getConverterDefinitions();
    if (converterDefinitions.isEmpty())
        return;

    int row = modelIndex.row();

    if (row < 0)
        return;

    auto dlg = new ConverterConfigurationEditDialog(this);
    if(row < converterDefinitions.count()) {
        dlg->setConverterDefinition(converterDefinitions.at(row));
        int result = dlg->exec();
        if(result == QDialog::Accepted) {
            converterDefinitions[row] = dlg->getConverterDefinition();
            convertersModel.setConverterDefinitions(converterDefinitions);
        }
    }
}

void ConverterConfigurationDialog::onDeleteRequested(const QModelIndex& modelIndex)
{
    int row = modelIndex.row();

    if(row < 0 )
        return;

    QVector<ConverterDefinition> converterDefinitions = convertersModel.getConverterDefinitions();
    if(row < converterDefinitions.count()) {
        converterDefinitions.removeAt(row);
        convertersModel.setConverterDefinitions(converterDefinitions);
    }
}

void ConverterConfigurationDialog::onCloneRequested(const QModelIndex& modelIndex)
{
    int row = modelIndex.row();

    if(row < 0 )
        return;

    QVector<ConverterDefinition> converterDefinitions = convertersModel.getConverterDefinitions();
    if(row < converterDefinitions.count()) {
        converterDefinitions.insert(row, converterDefinitions.at(row));
        convertersModel.setConverterDefinitions(converterDefinitions);
    }
}

void ConverterConfigurationDialog::onMoveUpRequested(const QModelIndex& modelIndex)
{
    int row = modelIndex.row();

    if(row < 1 ) {
        return;
    }

    QVector<ConverterDefinition> converterDefinitions = convertersModel.getConverterDefinitions();
    if(row < converterDefinitions.count()) {
        qSwap(converterDefinitions[row], converterDefinitions[row - 1]);
        convertersModel.setConverterDefinitions(converterDefinitions);
        tableView.selectRow(row - 1);
    }
}

void ConverterConfigurationDialog::onMoveDownRequested(const QModelIndex& modelIndex)
{

    int row = modelIndex.row();

    if(row < 0 ) {
        return;
    }

    QVector<ConverterDefinition> converterDefinitions = convertersModel.getConverterDefinitions();
    if(row < converterDefinitions.count() - 1) {
        qSwap(converterDefinitions[row], converterDefinitions[row + 1]);
        convertersModel.setConverterDefinitions(converterDefinitions);
        tableView.selectRow(row + 1);
    }
}
