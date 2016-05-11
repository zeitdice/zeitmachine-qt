#include "mainwindow.h"
#include <QApplication>
#include <QtPlugin>
#include <QScreen>

#if defined(Q_OS_LINUX) && defined(Q_NO_DEBUG)
Q_IMPORT_PLUGIN(QGtk2ThemePlugin)
Q_IMPORT_PLUGIN(QXcbIntegrationPlugin)
#endif

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    MainWindow w;
    w.show();

    return application.exec();
}
