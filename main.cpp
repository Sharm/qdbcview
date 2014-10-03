#include <QtGui/QApplication>
#include "qdbcinstance.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QDBCInstance i(&w);
    w.show();

    return a.exec();
}