#ifndef PUBLIC_DATA_H
#define PUBLIC_DATA_H

#include <QByteArray>
#include <QString>

typedef struct
{
    int x;
    int y;
}POS;

typedef struct
{
    int x;
    int y;
    int z;
}POS_3D;

static QByteArray public_to_msg(const QByteArray& command, const QByteArray& data)
{
    quint8 size_c = command.toStdString().length();
    quint8 size_d = data.size();

    QByteArray cmd(command.toStdString().c_str(), size_c);
    QByteArray res;
    res.append(size_c);
    res.append(size_d);
    res.append(cmd);
    res.append(data);

    return res;
}

static void public_to_data(QString& command, QByteArray& data, const QByteArray& data_in)
{
    const char* data_in_begin = data_in.data();
    quint8 size_c = *(data_in_begin + 0);
    quint8 size_d = *(data_in_begin + 1);

    command = QByteArray(data_in_begin + 2, size_c);
    data = QByteArray(data_in_begin + 2 + size_c, size_d);
}

#if __cplusplus < 201402L
  #error "not for C++11"
#else
template<typename F, typename C>
auto smart_bind(F f, C* c)
{
    return [c, f](auto&&... args)
    {
        return (c->*f)(std::forward<decltype(args)>(args)...);
    };
}
#endif




#endif // PUBLIC_DATA_H
