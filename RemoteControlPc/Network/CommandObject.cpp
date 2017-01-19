#include "CommandObject.h"
#include <QDebug>
#include "PUBLIC_DATA.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace MouseNM
{
#ifdef Q_OS_WIN
void MouseMove(int x, int y)//鼠标移动到指定位置
{
    double fScreenWidth = ::GetSystemMetrics(SM_CXSCREEN) - 1;//获取屏幕分辨率宽度
    double fScreenHeight = ::GetSystemMetrics(SM_CYSCREEN) - 1;//获取屏幕分辨率高度
    double fx = x*(65535.0f / fScreenWidth);
    double fy = y*(65535.0f / fScreenHeight);
    INPUT  Input = { 0 };
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    Input.mi.dx = fx;
    Input.mi.dy = fy;
    SendInput(1, &Input, sizeof(INPUT));
}

void MouseLeftDown()//鼠标左键按下
{
    INPUT  Input = { 0 };
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &Input, sizeof(INPUT));
}

void MouseLeftUp()//鼠标左键放开
{
    INPUT  Input = { 0 };
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &Input, sizeof(INPUT));

}

void MouseRightDown()//鼠标右键按下
{
    INPUT  Input = { 0 };
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
    SendInput(1, &Input, sizeof(INPUT));
}

void MouseRightUp()//鼠标右键放开
{
    INPUT  Input = { 0 };
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
    SendInput(1, &Input, sizeof(INPUT));
}
#endif
}

CommandObject::CommandObject(QObject *parent) : QObject(parent)
{
    m_method = new Method;
    m_thread = new QThread;

    connect(this, SIGNAL(inCommand(QByteArray)), m_method, SLOT(onCommandIn(QByteArray)), Qt::QueuedConnection);
    m_method->moveToThread(m_thread);
    m_thread->start();
}

CommandObject::~CommandObject()
{
    m_thread->quit();
    m_thread->wait(300);

    m_method->deleteLater();
    m_thread->deleteLater();
}

Method::Method(QObject *parent) : QObject(parent)
{

}

Method::~Method()
{
    //
}

void Method::onCommandIn(const QByteArray& inommand)
{
    QString command;
    QByteArray data;

    public_to_data(command, data, inommand);

    qDebug() << command;

    if (command.toLower() == "power off")
    {
        poweroff(data);

        POS pos;
        pos.x = 100;
        pos.y = 100;
        QByteArray cb((char*)&pos, sizeof(pos));

        curseMove(cb);

        return;
    }

    if (command.toLower() == "curse move")
    {
        poweroff(data);
        return;
    }
}

void Method::poweroff(const QByteArray& data)
{
    qDebug() << __FUNCTION__;
    Q_UNUSED(data);

#ifdef linux
    system("poweroff");
#endif

#ifdef __WIN32
    //system("shutdown -s -t 0");
#endif

#if 0
    system("shutdown -s -t 0");

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

#endif
}

void Method::curseMove(const QByteArray& data)
{
    qDebug() << __FUNCTION__;
    POS pos;

    if (sizeof(pos) != data.length())
    {
        qDebug() << __FUNCTION__ << " data error!";
        return;
    }

    memcpy(&pos, data.data(), data.length());

    MouseNM::MouseMove(pos.x, pos.y);
}

void Method::curseClick(const QByteArray& data)
{
    Q_UNUSED(data);
}


