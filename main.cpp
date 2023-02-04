#include "mainwindow.h"
#include "loggingcategories.h"
#include <QDebug>

QScopedPointer<QFile>   m_logFile;

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

int main(int argc, char *argv[])
{

    m_logFile.reset(new QFile("logFile.log"));
    m_logFile.data()->open(QFile::Append | QFile::Text);
    qInstallMessageHandler(messageHandler);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();

}

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{

    QTextStream out(m_logFile.data());

    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ");

    switch (type)
    {
    case QtInfoMsg:     out << "INF ";
        break;
    }

    out << context.category << ": " << msg << endl;

    out.flush();
}
