#include "ui/mainwindow.h"
#include <QFile>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString stylePath = ":/styles/resources/themes/dark.qss";
    QFile styleFile(stylePath);

    qInfo() << "Attempting to load stylesheet from:" << stylePath;

    if (!styleFile.exists()) {
        qCritical() << "CRITICAL ERROR: Resource file does NOT exist at path:" << stylePath;
        qCritical() << "Please check .qrc file definition and ensure the project was REBUILT.";
    }
    else if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&styleFile);
        QString styleSheet = stream.readAll();
        if (styleSheet.isEmpty()) {
            qWarning() << "WARNING: Stylesheet file is empty:" << stylePath;
        } else {
            a.setStyleSheet(styleSheet);
            qInfo() << "Stylesheet successfully loaded and applied from:" << stylePath;
        }
        styleFile.close();
    } else {
        qCritical() << "CRITICAL ERROR: Could not open resource file:" << stylePath << " Error: " << styleFile.errorString();
    }

    MainWindow w;
    w.show();
    return a.exec();
}
