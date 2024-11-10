#ifndef BANCODEDADOS_H
#define BANCODEDADOS_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QCryptographicHash>

class Bancodedados: public QObject
{
    Q_OBJECT
public:
    explicit Bancodedados(QSqlDatabase database);
    ~Bancodedados();
    bool criarTabelas();
    bool inserirCandidato(const QString &nome, const QString &vice, const QString &partido, int numero, const QString &cargo, QByteArray &imagem, QByteArray &imagemvice);
    bool inserirEleitor(const QString &nome, QString &ra);
    bool registrarVoto(int idCandidato, QByteArray idEleitor, const QString &cargo);
    QString nomeCandidato(const QString &matricula);
    QSqlQuery obterResultados();
    QSqlDatabase getDatabase() const;
    QSqlDatabase createDatabaseConnection();
    bool candidato(int numero, QString &nome, QString &nomevice, QString &partido, QByteArray &imagem, QByteArray &imagemvice);
    void inserirAdministradores();

private:
    QSqlDatabase db;
};

#endif // BANCODEDADOS_H
