#ifndef REMOTECONTROLCLIENTGUI_H
#define REMOTECONTROLCLIENTGUI_H

#include <QMainWindow>

namespace Ui {
class RemoteControlClientGui;
}

#include <QResizeEvent>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QTimer>

class RemoteControlClientGui : public QMainWindow
{
    Q_OBJECT

public:
    explicit RemoteControlClientGui(QWidget *parent = 0);
    ~RemoteControlClientGui();

private:
    Ui::RemoteControlClientGui *ui;

    QUdpSocket* m_scanSocket;
    QTcpSocket* m_clientSocket;

    char m_buff[1024];

    virtual void resizeEvent(QResizeEvent* e);

    QString m_hostName;
    QStringList m_ip;

    QTimer m_connectTimer;

private slots:
    void readPendingDatagrams();
    void connectToServer();
    void ReadDataFromSocket();
    void onDisConnection();
    void on_toolButton_clicked();
};

#endif // REMOTECONTROLCLIENTGUI_H
