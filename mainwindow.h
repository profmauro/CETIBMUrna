#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QDebug>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QObject>
#include <QtGlobal>
#include <QFileDialog>
#include <QIODevice>
#include <QString>
#include <QFileInfo>
#include <QPixmap>
#include <QCryptographicHash>
#include <QFile>
#include <QScopedPointer>
#include <QVariant>
#include <QTextStream>
#include <QDateTime>
#include <QLoggingCategory>
#include "loggingcategories.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent= 0);
    ~MainWindow();

    bool loggedIn;
    bool loggedOn;
    bool candidatoIn;
    bool votacao = false;
    bool Login(QString u, QString p);
    bool Candidato(QString prefeito, QString vereador);

    QString filename;
    QString file;
    QSqlTableModel *tableModel;

public slots:

private slots:

    void logout();

    void on_loginButton_clicked();

    void on_votar_clicked();

    void on_corrige_clicked();

    void on_corrige_2_clicked();

    void on_confirma_clicked();
    
    void on_winStack_currentChanged(int index);

    void on_branco_clicked();

    void on_zerarVotacao_clicked();

    void on_apurarVotos_clicked();

    void on_cadastroCandidato_clicked();

    void on_logout_adm_clicked();

    void on_cadastrarCandidato_clicked();

    void on_logEvent_clicked();

    void on_upload_foto_clicked();

    void on_logout_aud_clicked();

    void on_eleitor_clicked();


    void on_decriptar_clicked();

    void on_recontagem_clicked();

private:
    Ui::MainWindow      *ui1;
    QString             prefeito;
    QString             vereador;
    QString             inscricao;
    QString             senha;
    QString             senhadecrypt;
    QByteArray          hashed;
    QTimer              *timer;
};

#endif // MAINWINDOW_H
