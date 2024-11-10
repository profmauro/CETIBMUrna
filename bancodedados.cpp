#include "bancodedados.h"
#include <QSqlError>
#include <QDebug>

Bancodedados::Bancodedados(QSqlDatabase database) : db(database) {
    if(!db.isOpen()){
        if(!db.open()){
            qDebug() << "Não foi possível abrir o banco de dados:" << db.lastError().text();
        }
    }
}

Bancodedados::~Bancodedados(){
    if(db.open()){
        db.close();
    }
}

QSqlDatabase Bancodedados::getDatabase() const {
    return db;
}

bool Bancodedados::criarTabelas() {
    QSqlQuery query(db);

    if(!query.exec("CREATE TABLE IF NOT EXISTS administradores (id INTEGER PRIMARY KEY, nome TEXT, ra TEXT, cargoeleicao INTEGER, password TEXT)")){
        return false;
    }else{

    }


    if (!query.exec("CREATE TABLE IF NOT EXISTS eleitores (id INTEGER PRIMARY KEY, nome TEXT, ra TEXT, votou INTEGER)")) {
        //qDebug() << "Erro ao criar tabela eleitores:" << query.lastError().text();
        return false;
    }

    if (!query.exec("CREATE TABLE IF NOT EXISTS candidatos (id INTEGER PRIMARY KEY, nome TEXT, nomevice TEXT, partido TEXT, numero INTEGER, cargo TEXT, imagem BLOB, imagemvice BLOB)")) {
        //qDebug() << "Erro ao criar tabela eleitores:" << query.lastError().text();
        return false;
    }else{
        if(!query.exec("INSERT INTO candidatos (nome, numero, cargo) VALUES ('BRANCO', 202401, 'BRANCO')")){
            return false;
        }

        if(!query.exec("INSERT INTO candidatos (nome, numero, cargo) VALUES ('NULO', 202402, 'NULO')")){
            return false;
        }

    }

    if (!query.exec("CREATE TABLE IF NOT EXISTS votos_prefeito (id INTEGER PRIMARY KEY, idCandidato INTEGER, idEleitor TEXT)")) {
        //qDebug() << "Erro ao criar tabela votos:" << query.lastError().text();
        return false;
    }

    if (!query.exec("CREATE TABLE IF NOT EXISTS votos_vereador (id INTEGER PRIMARY KEY, idCandidato INTEGER, idEleitor TEXT)")) {
        //qDebug() << "Erro ao criar tabela votos:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Bancodedados::inserirCandidato(const QString &nome, const QString &vice, const QString &partido, int numero, const QString &cargo, QByteArray &imagem, QByteArray &imagemvice) {
    QSqlQuery query(db);
    query.prepare("INSERT INTO candidatos (nome, nomevice, partido, numero, cargo, imagem, imagemvice) VALUES (:nome, :nomevice, :partido, :numero, :cargo, :imagem, :imagemvice)");
    query.bindValue(":nome", nome);
    query.bindValue(":nomevice", vice);
    query.bindValue(":partido", partido);
    query.bindValue(":numero", numero);
    query.bindValue(":cargo", cargo);
    query.bindValue(":imagem", imagem);
    query.bindValue(":imagemvice", imagemvice);
    if (!query.exec()) {
        qDebug() << "Erro ao inserir candidato:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Bancodedados::inserirEleitor(const QString &nome, QString &ra) {
    QByteArray matricula = QCryptographicHash::hash(ra.toUtf8(), QCryptographicHash::Sha3_512).toHex();
    QSqlQuery query(db);
    query.prepare("INSERT INTO eleitores (nome, ra, votou) VALUES (:nome, :ra, 0)");
    query.bindValue(":nome", nome);
    query.bindValue(":ra", matricula);
    if (!query.exec()) {
        //qDebug() << "Erro ao inserir eleitor:" << query.lastError().text();
        return false;
    }
    return true;
}

bool Bancodedados::registrarVoto(int idCandidato, QByteArray idEleitor, const QString &cargo) {
    QString tabela = "votos_" + cargo;
    QSqlQuery query(db);
    QString sql = QString("INSERT INTO %1 (idCandidato, idEleitor) VALUES (:idCandidato, :idEleitor)").arg(tabela);
    query.prepare(sql);
    query.bindValue(":idCandidato", idCandidato);
    query.bindValue(":idEleitor", idEleitor);
    if (!query.exec()) {
        qDebug() << "Erro ao registrar voto:" << query.lastError().text();
        return false;
    }
    return true;
}

QSqlQuery Bancodedados::obterResultados() {
    QSqlQuery query(db);
    query.exec("SELECT candidatos.nome, COUNT(votos.id) as votos FROM votos JOIN candidatos ON votos.idCandidato = candidatos.id GROUP BY candidatos.nome");
    return query;
}

bool Bancodedados::candidato(int numero, QString &nome, QString &nomevice, QString &partido, QByteArray &imagem, QByteArray &imagemvice)
{
    bool existcadidato = false;
    QSqlQuery numeroexists(db);
    numeroexists.prepare("SELECT nome, nomevice, partido, imagem, imagemvice FROM candidatos WHERE numero = (:numero)");
    numeroexists.bindValue(":numero", numero);

    if (numeroexists.exec())
    {
        if (numeroexists.next())
        {
            nome = numeroexists.value("nome").toString();
            nomevice = numeroexists.value("nomevice").toString();
            partido = numeroexists.value("partido").toString();
            imagem = numeroexists.value("imagem").toByteArray();
            imagemvice = numeroexists.value("imagemvice").toByteArray();
            existcadidato = true;
        }
    }
    return existcadidato;
}

QString Bancodedados::nomeCandidato(const QString &matricula){
    QSqlQuery query(db);
    QByteArray ra;
    QString nome;
    ra = QCryptographicHash::hash(matricula.toUtf8(), QCryptographicHash::Sha3_512).toHex();
    query.prepare("SELECT nome FROM eleitores WHERE ra = (:matricula)");
    query.bindValue(":matricula", ra);
    if(query.exec()){
        if(query.next()){
            nome = query.value("nome").toString();
        }
    }
    return nome;
}
void Bancodedados::inserirAdministradores(){
    QSqlQuery query(db);
    QByteArray password, ra;
    password = QCryptographicHash::hash("Admin", QCryptographicHash::Sha3_512).toHex();
    ra = QCryptographicHash::hash("2024777", QCryptographicHash::Sha3_512).toHex();
    query.prepare("INSERT INTO administradores (id, nome, ra, cargoeleicao, password) VALUES (202401, 'ADMIN', :ra, 2024777, :password)");
    query.bindValue(":password", password);
    query.bindValue(":ra", ra);
    if(!query.exec()){
        qDebug() << query.lastError();
    }
    password = QCryptographicHash::hash("2024700", QCryptographicHash::Sha3_512).toHex();
    ra = QCryptographicHash::hash("2024700", QCryptographicHash::Sha3_512).toHex();
    query.prepare("INSERT INTO administradores (nome, ra, cargoeleicao, password) VALUES ('auditor', :ra, 2024700, :password)");
    query.bindValue(":password", password);
    query.bindValue(":ra", ra);
    if(!query.exec()){
        qDebug() << query.lastError();
    }
}
