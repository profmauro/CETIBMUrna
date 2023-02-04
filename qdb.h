#ifndef QDB_H
#define QDB_H

#include <QSqlDatabase>
#include <QSqlQuery>

class QDB
{
public:
    QDB();
    ~QDB();
    bool Connect();
    bool Disconnect();
    //QSqlQuery Query(QString q);
    QSqlDatabase db;
    bool dbstate;

private:
};

namespace QDBMysql {
    class DB : public QDB {};
}

#endif // QDB_H
