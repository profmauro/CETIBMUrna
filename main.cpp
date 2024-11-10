#include "mainwindow.h"
#include <QDebug>
#include "bancodedados.h"

QSqlDatabase createDatabaseConnection(){
    QString dbName = "votacao.db";
    QSqlDatabase banco = QSqlDatabase::addDatabase("QSQLITE", "main_connection");
    banco.setDatabaseName(dbName);
    if (!banco.open()){
        qDebug() << "fedeu!";
    }
    return banco;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSqlDatabase db = createDatabaseConnection();
    Bancodedados banco(db);
    if(!banco.criarTabelas()){
        return -1;
    }
    MainWindow w(db);
    w.show();

    return a.exec();

}
