#ifndef CONTROLPANNEL_H
#define CONTROLPANNEL_H

#include <QMainWindow>
class QMenu;
class QAction;
class QSystemTrayIcon;

class ControlPannel : public QObject
{
    Q_OBJECT

public:
    explicit ControlPannel(QObject *parent = 0);
    virtual ~ControlPannel();

public slots:
    void onConnect(bool res);

signals:
    void stop_start();

private:
    QMenu* m_menu;
    QAction* m_stopAction;
    QSystemTrayIcon* m_trayIcon;

private slots:
    void exit();
    void do_stop_start();
};

#endif // CONTROLPANNEL_H
