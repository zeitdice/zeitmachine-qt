#include "mainwindow.h"
#include <QApplication>
#include <QtPlugin>
#include <QScreen>


// Statically include platform plugin

#if defined(Q_OS_WIN)
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#endif

#if defined(Q_OS_MAC)
Q_IMPORT_PLUGIN(QCocoaIntegrationPlugin)
#endif

#if defined(Q_OS_LINUX) && defined(Q_NO_DEBUG)
Q_IMPORT_PLUGIN(QXcbIntegrationPlugin)
#endif

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    MainWindow w;
    w.show();

    return application.exec();
}
