#include "RemoteControlClientGui.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RemoteControlClientGui w;

#ifdef Q_OS_WIN
    w.show();
    w.setFixedSize(1080, 1920);
#endif

#ifdef Q_OS_ANDROID
    w.showFullScreen();
#endif

    return a.exec();
}
