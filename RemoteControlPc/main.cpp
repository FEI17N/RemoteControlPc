#include "ControlPannel.h"
#include <QApplication>

#include "Network/Network.h"

#include <QtSingleApplication>
#include <QDebug>

#include <QTranslator>

int main(int argc, char *argv[])
{
    QtSingleApplication a("FX REMOTE CONTROL PC", argc, argv);

    if (a.isRunning())
    {
        qDebug() << QObject::tr("FX REMOTE CONTROL PC IS RUNNING");
        return 0;
    }

    QTranslator translator;
    translator.load(":/zh_cn.qm");
    a.installTranslator(&translator);

    ControlPannel w;
    Network network;

    QObject::connect(&w, SIGNAL(stop_start()), &network, SLOT(stop_start()));
    QObject::connect(&network, SIGNAL(connected(bool)), &w, SLOT(onConnect(bool)));

    network.startLisnten();

    return a.exec();
}
