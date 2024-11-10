#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QDebug>
#include <QSqlRecord>
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
#include "bancodedados.h"
#include <QLineEdit>
#include <QBuffer>
#include <QGridLayout>
#include <QSqlError>
#include <QSqlQueryModel>
#include "eleitor.h"
#include <QListWidget>
#include <QProgressBar>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QSqlDatabase db, QWidget *parent= 0);
    ~MainWindow();

    bool loggedIn;
    bool loggedOn;
    bool candidatoIn;
    bool votacao = false;
    bool Login(QString u, QString p);

protected:
    void closeEvent(QCloseEvent *event) override;

public slots:

private slots:

    void logout();

    void on_loginButton_clicked();

    void on_zerarVotacao_clicked();

    void on_apurarVotos_clicked();

    void on_cadastroCandidato_clicked();

    void on_logout_adm_clicked();

    void on_cadastrarCandidato_clicked();

    void on_upload_foto_clicked();

    void on_logout_aud_clicked();

    void on_recontagem_clicked();

    void checkFocus1();

    void checkFocus2();

    void on_branco_vereador_clicked();

    void on_branco_prefeito_clicked();

    void on_corrige_vereador_clicked();

    void on_corrige_prefeito_clicked();

    void on_votar_vereador_clicked();

    void mostrarErro(const QString &mensagem);

    void on_radioprefeito_clicked(bool checked);

    void on_radiovereador_clicked(bool checked);

    void on_upload_foto_vice_clicked();

    void on_votar_prefeito_clicked();

    void receber_matricula(const QString &matricula);

    void abrir_janela_eleitor();

    void iniciando_votacao();

    void on_inserieleitores_clicked();

    void processSelectedFiles();

    void on_cadastrareleitor_clicked();

    void on_pushButton_clicked();

signals:
    void enviando_processo(const QString &processo, bool terminou);

    void confirmado_matricula(const QString &nome);

private:
    Ui::MainWindow      *ui1;
    QString             inscricao;
    QString             senha;
    QString             senhadecrypt;
    QByteArray          hashed;

    Bancodedados db;

    void voto_branco(const QString &cargo);

    void limparvereador();

    void limparprefeito();

    QPushButton *button[9];

    void inserirbutton(QGridLayout *gridbutton);

    Eleitor *eleitor;

    void vereadorvisivel(bool visivel);

    QString correctPassword = "somenteeumace";

    QListWidget *listWidget;

    QProgressBar *progressBar;

    bool processFile(const QString& filePath, QProgressBar *progressBar);
};

#endif // MAINWINDOW_H
