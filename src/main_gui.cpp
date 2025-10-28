#include <QApplication>
#include "gui/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setApplicationName("Password File Manager");
    QApplication::setOrganizationName("PasswordFileManager");

    MainWindow window;
    window.show();

    return app.exec();
}