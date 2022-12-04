#include "WoodLightSetup.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WoodLightSetup w;
    w.show();
    return a.exec();
}
