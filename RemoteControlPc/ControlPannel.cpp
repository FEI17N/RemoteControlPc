#include "ControlPannel.h"

#include <QSystemTrayIcon>
#include <QApplication>
#include <QMenu>

ControlPannel::ControlPannel(QObject *parent) :
    QObject(parent)
{
    QIcon icon = QIcon(":/TRAY.png");
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(icon);
    m_trayIcon->setToolTip(tr("FX AUTO POWER OFF"));
    QString titlec=tr("FX AUTO POWER OFF");
    QString textc=tr("FX：REMOTE POWER OFF PC FROM CELLPHONE");
    m_trayIcon->show();
    m_trayIcon->showMessage(titlec,textc,QSystemTrayIcon::Information,5000);

    m_menu = new QMenu;

    m_stopAction = m_menu->addAction(QIcon(), tr("STOP"));
    bool res = connect(m_stopAction, SIGNAL(triggered(bool)), this, SLOT(do_stop_start()));
    Q_ASSERT(res);

    res = connect(m_menu->addAction(QIcon(), tr("EXIT")), SIGNAL(triggered(bool)), this, SLOT(exit()));
    Q_ASSERT(res);

    m_trayIcon->setContextMenu(m_menu);

    setProperty("isListen", true);
}

ControlPannel::~ControlPannel()
{
    m_menu->deleteLater();
}

void ControlPannel::onConnect(bool netConn)
{
    bool res = property("isListen").toBool();

    if (res)
    {
        m_trayIcon->setIcon(netConn ? QIcon(":/TRAY_C.png") : QIcon(":/TRAY.png"));
    }
    else
    {
        m_trayIcon->setIcon(QIcon(":/TRAY_S.png"));
    }
}

void ControlPannel::exit()
{
    qApp->quit();
}

void ControlPannel::do_stop_start()
{
    bool res = (!property("isListen").toBool());
    setProperty("isListen", res);

    m_stopAction->setText(res ? tr("STOP") : tr("START"));
    m_trayIcon->setIcon(res ? QIcon(":/TRAY.png") : QIcon(":/TRAY_S.png"));

    emit stop_start();
}
