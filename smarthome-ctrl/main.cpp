#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    /* for LCD display, can changed to own resolution */
    w.resize(320, 240);
    w.show();

    return a.exec();
}
