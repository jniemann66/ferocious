#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationVersion(APP_VERSION);

    // retrieve and apply Stylesheet:
    QFile ss(":/ferocious.qss");
    if(ss.open(QIODevice::ReadOnly | QIODevice::Text)){
        a.setStyleSheet(ss.readAll());
        ss.close();
    }else{
        qDebug() << "Couldn't open stylesheet resource";
    }

    MainWindow w;
    w.show();
    return a.exec();
}
