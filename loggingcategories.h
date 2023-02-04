#ifndef LOGGER_H
#define LOGGER_H

#include <QLoggingCategory>
#include <QFile>
#include <QCryptographicHash>
#include <QSqlQuery>

Q_DECLARE_LOGGING_CATEGORY(logInfo)

class LoggingCategories
{

public:
    explicit LoggingCategories();
    ~LoggingCategories();

public slots:

    void gerarHash();

};

#endif // LOGGER_H
