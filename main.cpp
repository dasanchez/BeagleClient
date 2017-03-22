#include "beagleclient.h"
#include <QApplication>

int main(int argc, char *argv[])
{
//    SDL_SetMainReady();
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
    QApplication a(argc, argv);
    BeagleClient w;
//    w.show();
//    SDL_Quit();
    return a.exec();
}
