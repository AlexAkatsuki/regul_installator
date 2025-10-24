#include <QApplication>
#include <QTranslator>
#include <QLocale>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("Regul_Installer");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Regul");

    MainWindow window;
    window.show();

    return app.exec();
}
