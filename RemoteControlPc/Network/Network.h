#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QList>
#include <QTimer>

class QTcpServer;
class QTcpSocket;
class QUdpSocket;
class QAbstractSocket;

class protocol_parse;

class CommandObject;

class Network : public QObject
{
    Q_OBJECT
public:
    Network(QObject* parent = nullptr);
    virtual ~Network();

public slots:
    void startLisnten();
    void stop_start();

signals:
    void connected(bool res);

private:
    static void newMessageCome(char* msg, int len);
    //static void powerOff();

    QTcpServer* m_tcpServer;
    QUdpSocket* m_udpSocket;
    QTcpSocket* m_client;
    QTimer m_brocastTimer;
    QTimer m_heartTimer;
    protocol_parse *m_parse;

    void writeMessage(QAbstractSocket* socket, char* msg, int length);

private slots:
    void ReadDataFromSocket();
   void onNewConnection();
   void onDisConnection();
   void brocastInfo();
   void heartInfo();


};

#endif // NETWORK_H
