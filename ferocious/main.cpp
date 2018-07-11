#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationVersion(APP_VERSION);

    // activate anti-aliasing on all fonts:
    QFont font = QApplication::font();
    font.setStyleStrategy(QFont::PreferAntialias);
    a.setFont(font);

    // retrieve and apply factory Stylesheet:
    QFile ss(":/ferocious.css");
    if(ss.open(QIODevice::ReadOnly | QIODevice::Text)) {
        a.setStyleSheet(ss.readAll());
        ss.close();
    } else {
        qDebug() << QString{"Couldn't open stylesheet resource: %1"}.arg(ss.fileName());
    }

    MainWindow w;
    w.show();
    return a.exec();
}
