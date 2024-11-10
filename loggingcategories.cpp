#include "loggingcategories.h"

Q_LOGGING_CATEGORY(logInfo,     "Info")

LoggingCategories::LoggingCategories()
{

}

LoggingCategories::~LoggingCategories()
{

}

void LoggingCategories::gerarHash()
{
        QFile file("logFile.log");
        file.open(QIODevice::ReadWrite | QIODevice::Text);
        QByteArray fileData = file.readAll();

        QByteArray res = QCryptographicHash::hash(fileData, QCryptographicHash::Sha3_512).toHex();

        QSqlQuery hash;
        hash.prepare("UPDATE hash SET hash_atual = (:hash) WHERE hash_nome = 'log'");
        hash.bindValue(":hash", res);
        hash.exec();

        file.close();
}
