#include "Network.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QHostAddress>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include <QDebug>

extern "C" {
#include "protocol_parse.h"
}

const quint16 listenPort = 7430;

Network::Network(QObject* parent)
    : QObject(parent)
{
    m_tcpServer = new QTcpServer(this);
    m_tcpServer->setMaxPendingConnections(1);
    m_udpSocket = new QUdpSocket(this);

    connect(m_tcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));

    setProperty("TCO_CONNECT", false);

    m_parse = new protocol_parse;

    m_parse->protocol_parse_set_callback(newMessageCome);

    connect(&m_brocastTimer, SIGNAL(timeout()), this, SLOT(brocastInfo()));
    connect(&m_heartTimer, SIGNAL(timeout()), this, SLOT(heartInfo()));

    QString localHostName = QHostInfo::localHostName();
    setProperty("localHostName", localHostName);

    setProperty("LISTEN", false);
}

Network::~Network()
{

     m_parse->protocol_parse_set_callback(NULL);
     m_tcpServer->close();
    delete m_parse;
}

void Network::stop_start()
{
    bool res = !property("LISTEN").toBool();
    if (!res)
    {
        if (property("TCO_CONNECT").toBool())
        {
            setProperty("TCO_CONNECT", false);
            m_client->close();
            m_client->deleteLater();
        }

        if (m_tcpServer->isListening())
        {
            m_tcpServer->close();
        }

        m_brocastTimer.stop();
    }
    else
    {
        startLisnten();
    }
    setProperty("LISTEN", res);
}

void Network::startLisnten()
{
    if (m_tcpServer->isListening())
    {
        m_tcpServer->close();
    }

    m_tcpServer->listen(QHostAddress::Any, listenPort);
    setProperty("LISTEN", true);

    m_brocastTimer.start(3000);
}

void Network::newMessageCome(char* msg, int len)
{
    QString inMsg(QByteArray(msg, len));
    qDebug() << __FUNCTION__ << " " <<  inMsg;

    if (inMsg.toLower() == "power off")
    {
        powerOff();
    }
}

void Network::onNewConnection()
{
    m_client = m_tcpServer->nextPendingConnection();
    //写入验证

    connect(m_client,SIGNAL(readyRead()),this,SLOT(ReadDataFromSocket()));
    connect(m_client,SIGNAL(disconnected()),m_client,SLOT(deleteLater()));
    connect(m_client,SIGNAL(disconnected()),this,SLOT(onDisConnection()));
    m_client->write(QString("Server").toLatin1());
    m_client->flush();

    setProperty("TCO_CONNECT", true);
    m_heartTimer.start(2000);

    emit connected(true);
}

void Network::ReadDataFromSocket()
{
    QByteArray rawreply= static_cast<QTcpSocket*>(sender())->readAll();
    for (int i = 0; i < rawreply.size(); ++i)
    {
        m_parse->protocol_parse_in(rawreply[i]);
    }
}

void Network::writeMessage(QAbstractSocket* socket, char* inmsg, int length)
{
    int out_length = 0;
    char* msg = m_parse->protocol_parse_to_message(length, inmsg, &out_length);
    socket->write(QByteArray(msg, out_length));
}

void Network::onDisConnection()
{
    m_heartTimer.stop();
    emit connected(false);
}

void Network::brocastInfo()
{
    struct GetIpStruct
    {
        QList<QHostAddress> getLocalHostIP()
        {
          QList<QHostAddress> AddressList = QNetworkInterface::allAddresses();
          QList<QHostAddress> result;
          foreach(QHostAddress address, AddressList)
          {
              qDebug() << address.toString();
              if(address.protocol() == QAbstractSocket::IPv4Protocol &&
                 address != QHostAddress::Null &&
                 address != QHostAddress::LocalHost){
                  if (address.toString().contains("127.0.")){
                    continue;
                  }
                  result.append(address);
              }
          }
          return result;
        }
    };

    QString localHostName = QHostInfo::localHostName();
    setProperty("localHostName", localHostName);

    QStringList ips;
    GetIpStruct getIpStruct;
    QList<QHostAddress> has = getIpStruct.getLocalHostIP();
    for(int i = 0; i < has.size(); ++i)
    {
        if (!ips.contains(has[i].toString()))
        {
            ips.append(has[i].toString());
        }
    }

    QJsonObject json;
    json.insert("host", localHostName);
    QJsonArray array;
    for (int i = 0; i < ips.size(); ++i)
    {
        array.append(ips[i]);
    }

    json.insert("ip", array);

    QJsonDocument doc;
    doc.setObject(json);
    QString brMsg = QString(doc.toJson());

    brMsg.prepend("[json_head]");
    brMsg.append("[json_end]");

#if 1
    qDebug() << brMsg;
#endif

    m_udpSocket->writeDatagram
                (
                QByteArray(brMsg.toStdString().c_str(), brMsg.toStdString().length()),
                QHostAddress::Broadcast,
                listenPort
                );

}

void Network::heartInfo()
{
    if (property("TCO_CONNECT").toBool())
    {
        QString localHostName = property("localHostName").toString();
        writeMessage(m_client , (char*)localHostName.toStdString().c_str(), localHostName.size());
    }
}

void Network::powerOff()
{
    qDebug() <<__FUNCTION__;

#ifdef linux
    system("poweroff");
#endif

#ifdef __WIN32
    system("shutdown -s -t 0");
    /*
        HANDLE hToken;
        TOKEN_PRIVILEGES tkp;

        //获取进程标志
        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        {
            return ;
        }
        //获取关机特权的LUID
        LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,    &tkp.Privileges[0].Luid);
        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        //获取这个进程的关机特权
        AdjustTokenPrivileges(hToken, false, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
        if (GetLastError() != ERROR_SUCCESS)
        {
                return ;
        }
        // 强制关闭计算机
        if ( !ExitWindowsEx(EWX_SHUTDOWN , 0))  //关机
        {
            return ;
        }
        */
#endif
}

