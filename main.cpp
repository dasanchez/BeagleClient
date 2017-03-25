#include "beagleclient.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BeagleClient w;
    w.show();
    return a.exec();
}
