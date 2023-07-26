/*
* Copyright (C) 2016 - 2023 Judd Niemann - All Rights Reserved.
* You may use, distribute and modify this code under the
* terms of the GNU Lesser General Public License, version 2.1
*
* You should have received a copy of GNU Lesser General Public License v2.1
* with this file. If not, please refer to: https://github.com/jniemann66/ferocious
*/

#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

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
