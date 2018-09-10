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

typedef struct
{
    quint8 a0;
    quint8 a1;
}SIZED;

static QByteArray public_to_msg(const QByteArray& command, const QByteArray& data)
{
    quint8 size_c = command.length();
    quint16 size_d = data.size();

    Q_ASSERT(sizeof(size_d) == sizeof(SIZED));

    SIZED sized_d;
    memcpy(&sized_d, &size_d, sizeof(size_d));

    QByteArray cmd(command.data(), size_c);
    QByteArray res;
    res.append(size_c);
    res.append(sized_d.a0);
    res.append(sized_d.a1);
    res.append(cmd);
    res.append(data);

    return res;
}

static void public_to_data(QByteArray& command, QByteArray& data, const QByteArray& data_in)
{
    SIZED sized_d;
    const char* data_in_begin = data_in.data();
    quint8 size_c = *(data_in_begin + 0);
    //quint8 size_d = *(data_in_begin + 1);

    sized_d.a0 = *(data_in_begin + 1);
    sized_d.a1 = *(data_in_begin + 2);

    quint16 size_d;

    Q_ASSERT(sizeof(size_d) == sizeof(SIZED));
    memcpy(&size_d, &sized_d, sizeof(size_d));

    command = QByteArray(data_in_begin + 3, size_c);
    data = QByteArray(data_in_begin + 3 + size_c, size_d);
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
