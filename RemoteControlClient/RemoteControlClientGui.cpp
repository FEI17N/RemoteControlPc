#include "RemoteControlClientGui.h"
#include "ui_RemoteControlClientGui.h"

#include <QHostAddress>
#include <QDebug>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

#include "protocol_parse.h"

const quint16 listenPort = 7431;

RemoteControlClientGui::RemoteControlClientGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RemoteControlClientGui)
{
    ui->setupUi(this);

    m_parse = new protocol_parse;

    m_scanSocket = new QUdpSocket(this);
    m_clientSocket = new QTcpSocket(this);

#ifdef Q_OS_LINUX
    m_scanSocket->bind(listenPort, QUdpSocket::ShareAddress);
#else //Q_OS_WIN32
    m_scanSocket->bind(listenPort, QUdpSocket::ReuseAddressHint);
#endif
    connect(m_scanSocket, SIGNAL(readyRead()),this, SLOT(readPendingDatagrams()));

    setProperty("SERVER_CHANGE", false);
    setProperty("SERVER_CONNECT", false);

    connect(&m_connectTimer, SIGNAL(timeout()), this, SLOT(connectToServer()));
    m_connectTimer.start(2000);

    connect(m_clientSocket,SIGNAL(readyRead()),this,SLOT(ReadDataFromSocket()));
    connect(m_clientSocket,SIGNAL(disconnected()),this,SLOT(onDisConnection()));

    ui->debuger->hide();
    ui->toolButton->setText("");
}

RemoteControlClientGui::~RemoteControlClientGui()
{
    delete ui;
    delete m_parse;
}

void RemoteControlClientGui::resizeEvent(QResizeEvent* e)
{
    ui->label->setFixedSize(e->size().width() * 0.6, e->size().width() * 0.6);
    ui->toolButton->setFixedSize(e->size().width() * 0.3, e->size().width() * 0.3);

    ui->label->setPixmap(QPixmap::fromImage(QImage(":/TRAY.png").scaled(ui->label->size())));

    ui->topSpace->changeSize(20, e->size().height() * 0.1, QSizePolicy::Minimum, QSizePolicy::Fixed);
    ui->bottomSpace->changeSize(20, e->size().height() * 0.20, QSizePolicy::Minimum, QSizePolicy::Fixed);
}

void RemoteControlClientGui::readPendingDatagrams()
{
    quint64 len = m_scanSocket->readDatagram(m_buff, 1024);
    QByteArray content(m_buff, len);

    qDebug() << len << " " << content;

    if (len == 0)
    {
        ui->debuger->setText("EMPTY");
    }
    else
    {
        ui->debuger->setText(QString(content));
    }

    QByteArray jsonHead = QByteArray("[json_head]", sizeof("[json_head]") - 1);
    QByteArray jsonEnd = QByteArray("[json_end]", sizeof("[json_end]") - 1);


    if (content.contains(jsonHead) && content.contains(jsonEnd))
    {
        QString contentString(content);
        QStringList clist = contentString.split("[json_head]", QString::KeepEmptyParts);
        clist.removeFirst();
        contentString = clist.join("");
        clist = contentString.split("[json_end]", QString::KeepEmptyParts);
        clist.removeLast();
        contentString = clist.join("");

        ui->debuger->setText(contentString);

        QJsonDocument doc = QJsonDocument::fromJson(QByteArray(contentString.toStdString().c_str(), contentString.toStdString().length()));
        QJsonObject object = doc.object();
        QString hostName = object.value("host").toString();
        QJsonArray ips = object.value("ip").toArray();

        QStringList ip;
        for (int i = 0; i < ips.size(); ++i)
        {
            ip.append(ips.at(i).toString());
        }

         ui->debuger->setText(hostName + "\n" + ip.join("\n"));

         if (m_hostName != hostName || ip != m_ip)
         {
             setProperty("SERVER_CHANGE", true);
             m_hostName = hostName;
             m_ip = ip;
         }
    }

}

void RemoteControlClientGui::connectToServer()
{
    if (property("SERVER_CHANGE").toBool() || !m_clientSocket->isOpen() || !property("SERVER_CONNECT").toBool())
    {
        ui->pcName->setText(m_hostName);

        if (property("SERVER_CHANGE").toBool())
        {
            setProperty("SERVER_CHANGE", false);
        }

        if (!m_clientSocket->isOpen())
        {
            setProperty("SERVER_CONNECT", false);
        }

        if (m_clientSocket->isOpen())
        {
            m_clientSocket->close();
            setProperty("SERVER_CONNECT", false);
        }

        bool connectSuccess = false;
        for (int i = 0; i < m_ip.size(); ++i)
        {
            m_clientSocket->connectToHost(m_ip[i], listenPort);
            bool res = m_clientSocket->waitForConnected(300);
            if (res)
            {
                connectSuccess = true;
                break;
            }
        }

        if (connectSuccess)
        {
            setProperty("SERVER_CONNECT", true);
            ui->label->setPixmap(QPixmap::fromImage(QImage(":/TRAY_C.png").scaled(ui->label->size())));
            ui->debuger->setText("CONNECT SUCCESS");
            return;
        }
        else
        {
            setProperty("SERVER_CONNECT", false);
            ui->debuger->setText("CONNECT FAILURE");
            ui->label->setPixmap(QPixmap::fromImage(QImage(":/TRAY_S.png").scaled(ui->label->size())));
            return;
        }
    }
}

void RemoteControlClientGui::ReadDataFromSocket()
{
    QByteArray array = m_clientSocket->readAll();
    ui->debuger->setText(array);
}

void RemoteControlClientGui::onDisConnection()
{
    setProperty("SERVER_CONNECT", false);
    ui->debuger->setText("CONNECT FAILURE");
    ui->label->setPixmap(QPixmap::fromImage(QImage(":/TRAY_S.png").scaled(ui->label->size())));
}

void RemoteControlClientGui::on_toolButton_clicked()
{
   if (m_clientSocket->isOpen())
   {
       char command[] = {"POWER OFF"};
       int out_length = 0;
       char* msg = m_parse->protocol_parse_to_message(sizeof(command), command, &out_length);
       QByteArray sendMsg(msg, out_length);
       m_clientSocket->write(sendMsg);
   }
}
