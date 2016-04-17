#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QString ConverterPath;

private slots:
    void on_StdoutAvailable();
    void on_ConverterStarted();
    void on_ConverterFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void on_browseInfileButton_clicked();
    void on_convertButton_clicked();
    void on_InfileEdit_editingFinished();
    void on_browseOutfileButton_clicked();
    void on_NormalizeCheckBox_clicked();
    void on_NormalizeAmountEdit_editingFinished();
    void on_BitDepthCheckBox_clicked();
    void on_OutfileEdit_editingFinished();

    void on_DitherCheckBox_clicked();

    void on_DitherAmountEdit_editingFinished();

private:
    Ui::MainWindow *ui;
    QProcess Converter;

    void PopulateBitFormats(const QString& fileName);
    bool fileExists(const QString& path);
    void writeSettings();
    void readSettings();
};

#endif // MAINWINDOW_H
