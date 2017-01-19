#ifndef COMMANDOBJECT_H
#define COMMANDOBJECT_H

#include <QObject>
#include <QThread>

class Method;

class CommandObject : public QObject
{
    Q_OBJECT
public:
    explicit CommandObject(QObject *parent = 0);
    virtual ~CommandObject();

signals:
    void inCommand(const QByteArray& command);

private:
    Method* m_method;
    QThread* m_thread;
};

class Method : public QObject
{
    Q_OBJECT
public:
    explicit Method(QObject *parent = 0);
    virtual ~Method();

public slots:
    void onCommandIn(const QByteArray& command);

private:
    void poweroff(const QByteArray& data);
    void curseMove(const QByteArray& data);
    void curseClick(const QByteArray& data);
};

#endif // COMMANDOBJECT_H
