#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Obywatel GCC");
    QCoreApplication::setApplicationName("Guitar Text");
    MainWindow w;
    w.show();
    
    return a.exec();
}
