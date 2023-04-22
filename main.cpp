#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include<QIcon>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/logo.ico"));
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "JBMN_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;

    w.show();

    return a.exec();
}

