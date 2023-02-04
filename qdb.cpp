#include "qdb.h"

QDB::QDB()
{

}

QDB::~QDB()
{
    Disconnect();
}

bool QDB::Connect()
{

    if (!this->db.isOpen())
    {
        this->db = QSqlDatabase::addDatabase("QMYSQL");
        this->db.setDatabaseName("urnas");
        this->db.setHostName("localhost");
        this->db.setPort(3306);
        this->db.setUserName("administrador");
        this->db.setPassword("testtest");
        setlocale(LC_ALL, "");

        if (!this->db.open())
        {
          return false;
        }
        else
        {
          return true;
        }
    }
    else
    {
        return false;
    }
}

bool QDB::Disconnect()
{
    if (this->db.isOpen())
    {
        this->db.close();
        return true;
    }
    else
    {
        return false;
    }
}
