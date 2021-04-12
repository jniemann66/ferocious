/*
* Copyright (C) 2016 - 2021 Judd Niemann - All Rights Reserved.
* You may use, distribute and modify this code under the
* terms of the GNU Lesser General Public License, version 2.1
*
* You should have received a copy of GNU Lesser General Public License v2.1
* with this file. If not, please refer to: https://github.com/jniemann66/ferocious
*/

#ifndef CONVERTERCONFIGURATIONDIALOG_H
#define CONVERTERCONFIGURATIONDIALOG_H

#include "converterdefinitionsmodel.h"
#include "fancylineedit.h"
#include "converterconfigurationeditdialog.h"

#include <QDialog>
#include <QLineEdit>
#include <QTableView>
#include <QMenu>
#include <QToolBar>

class ConverterConfigurationDialog : public QDialog
{
    Q_OBJECT

public:
    ConverterConfigurationDialog(QWidget *parent, Qt::WindowFlags f);
    QString getExpectedMainConverter() const;
    QString getMainConverterPath() const;
    QVector<ConverterDefinition> getConverterDefinitions() const;
    QRect getEditDialogGeometry() const;
    void setExpectedMainConverter(const QString &value);
    void setMainConverterPath(const QString &value);
    void setConverterDefinitions(const QVector<ConverterDefinition> &value);
    void setShowToolTips(bool value);
    void setEditDialogGeometry(const QRect &value);

public slots:

protected:
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onNewRequested(const QModelIndex &modelIndex);
    void onEditRequested(const QModelIndex &modelIndex);
    void onDeleteRequested(const QModelIndex &modelIndex);
    void onCloneRequested(const QModelIndex &modelIndex);
    void onMoveUpRequested(const QModelIndex &modelIndex);
    void onMoveDownRequested(const QModelIndex &modelIndex);
    void onRestoreDefaults();

private:
    QLabel* mainConverterLocationLabel;
    QLabel* additionalConvertersLabel;
    FancyLineEdit* mainConverterLocationEdit;
    QPushButton* browseButton;
    QMenu* contextMenu;
    QToolBar* contextToolBar;
    QTableView tableView;
    ConverterDefinitionsModel convertersModel;
    QString expectedMainConverter;
    QString mainConverterPath;
    QRect editDialogGeometry;
    bool showToolTips;

    void promptForResamplerLocation();
    void initMenu();
    void initToolBar();
};

#endif // CONVERTERCONFIGURATIONDIALOG_H
