#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qdb.h"
#include "loggingcategories.h"
#include "audio.h"
#include <QProcess>

QDBMysql::DB db;
LoggingCategories hash;
Audio audio;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui1(new Ui::MainWindow)
{
    ui1->setupUi(this);

    timer = new QTimer(this);
    db.dbstate = db.Connect();

    ui1->winStack->setCurrentIndex(0);
    ui1->prefeito_1->setValidator(new QIntValidator(0,9,this));
    ui1->prefeito_2->setValidator(new QIntValidator(0,9,this));
    ui1->vereador_1->setValidator(new QIntValidator(0,9,this));
    ui1->vereador_2->setValidator(new QIntValidator(0,9,this));
    ui1->vereador_3->setValidator(new QIntValidator(0,9,this));
    ui1->vereador_4->setValidator(new QIntValidator(0,9,this));
    ui1->vereador_5->setValidator(new QIntValidator(0,9,this));

}
MainWindow::~MainWindow()
{
    delete ui1;
}

/* TELA 1 LOGIN */

void MainWindow::on_loginButton_clicked()
{

    this->loggedIn = Login(ui1->usernameText->text(), ui1->passwordText->text());
    if(this->loggedIn)
    {
        QSqlQuery votou1(db.db);
        votou1.prepare("SELECT Votou,Rank FROM usuario WHERE Inscricao = (:inscricao)");
        votou1.bindValue(":inscricao", this->hashed);
        votou1.exec();

        QString votou;
        QString rank;

        while (votou1.next())
        {
            votou = votou1.value(0).toString();
            rank = votou1.value(1).toString();
        }

        if (votou =="Não" && rank =="3")
        {

            bool clearvotos = false;

            QSqlQuery votoslimpos(db.db);
            votoslimpos.prepare("SELECT Votos FROM candidato WHERE Votos != '0'");

            if (votoslimpos.exec())
            {
                if (votoslimpos.next())
                {
                    clearvotos = true;
                }
            }

            if (clearvotos)
            {
                votacao = false;
                ui1->loginLabel->setText("Impedido de iniciar Votação Candidatos com Votos >=1 ou Votacao Encerrada");
                qInfo(logInfo()) << "Existe candidato(s) com mais de um voto, impossivel inicia votacao ou Votacao Encerrada";
                hash.gerarHash();
            }
            if (clearvotos == false)
            {
                votacao =! votacao;
                if (votacao == true)
                {
                    ui1->loginLabel->setText("Votação Iniciada");
                    qInfo(logInfo()) << "Votação Iniciada Com Sucesso, Login Administrador, Auditor Desativado";
                    hash.gerarHash();
                }
                if (votacao == false)
                {
                    ui1->loginLabel->setText("Votação Encerada");
                    qInfo(logInfo()) << "Votação Encerrada, Login Administrador, Auditor Ativado";
                    hash.gerarHash();
                }
                qInfo(logInfo()) << "Todos candidatos com 0 votos";
                hash.gerarHash();
            }

            ui1->usernameText->setText("");
            ui1->passwordText->setText("");
        }
        if(votou == "Não" && rank == "0" && votacao==true)
        {
            ui1->loginLabel->setText("");
            ui1->winStack->setCurrentIndex(1);
            qInfo(logInfo()) << "Usuario Logou";
            hash.gerarHash();
        }

        if(rank =="1" && votacao==false)
        {
            ui1->loginLabel->setText("");
            ui1->winStack->setCurrentIndex(3);
            ui1->admStack->setCurrentIndex(0);
            qInfo(logInfo()) << "Administrador Logou";
            hash.gerarHash();
        }

        if(rank =="2" && votacao==false)
        {
            ui1->loginLabel->setText("");
            ui1->winStack->setCurrentIndex(4);
            //qInfo(logInfo()) << "Auditor Logou";
            //hash.gerarHash();
        }

        else if (votou == "Sim")
        {
            ui1->loginLabel->setText("Você já votou");
            qInfo(logInfo()) << "Tentativa de votar novamente impedido";
            hash.gerarHash();
        }
    }
    else
    {
        ui1->loginLabel->setText("Falha no Login: Usúario ou Senha/Inscrição Incorreto!");
        qInfo(logInfo()) << "Falha na Autenticação: Senha Errada";
        hash.gerarHash();
    }


}

bool MainWindow::Login(QString u, QString p)
{
    QByteArray hash = p.toUtf8();
    this->hashed = QCryptographicHash::hash(hash, QCryptographicHash::Sha3_512).toHex();
    bool exists = false;
    QSqlQuery checkQuery(db.db);
    checkQuery.prepare("SELECT * FROM usuario WHERE Nome = (:user) AND Inscricao = (:pw)");
    checkQuery.bindValue(":user", u);
    checkQuery.bindValue(":pw", hashed);

    if (checkQuery.exec())
    {
        if (checkQuery.next())
        {
            exists = true;
        }
    }
    return exists;
}

/* TELA 2 VOTACAO */


void MainWindow::on_votar_clicked()
{
    bool branco = false;

    if(ui1->prefeito_1->text() == "" || ui1->prefeito_2->text() == "" || ui1->vereador_1->text() == "" ||
            ui1->vereador_2->text() == "" || ui1->vereador_3->text() == "" || ui1->vereador_4->text() == "" ||
            ui1->vereador_5->text() == "")
    {
        branco = true;
    }

    if(branco)
    {
        ui1->text_error->setText("Numero em Falta");
    }

    else
    {
        this->candidatoIn = Candidato((ui1->prefeito_1->text() + ui1->prefeito_2->text()),(ui1->vereador_1->text()+ui1->vereador_2->text()+ui1->vereador_3->text()+ui1->vereador_4->text()+ui1->vereador_5->text()));

        if(this->candidatoIn)
        {
            ui1->text_error->setText("");
            ui1->winStack->setCurrentIndex(2);
        }
        else
        {
            ui1->text_error->setText("Numeros Invalidos");
        }
    }
}

bool MainWindow::Candidato(QString prefeito, QString vereador)
{
    bool exists1 = false;

    QSqlQuery numeroexists(db.db);
    numeroexists.prepare("SELECT * FROM candidato WHERE NumeroPartido = (:prefeito) OR NumeroPartido = (:vereador)");
    numeroexists.bindValue(":prefeito", prefeito);
    numeroexists.bindValue(":vereador", vereador);

    this->prefeito = prefeito;
    this->vereador = vereador;

    if (numeroexists.exec())
    {
        if (numeroexists.next())
        {
            exists1 = true;
        }
    }
    return exists1;
}


void MainWindow::on_branco_clicked()
{
    if(QMessageBox::Yes == QMessageBox(QMessageBox::Question,
                                       "Voto Branco", "Você tem certeza que deseja votar em branco?",
                                       QMessageBox::Yes|QMessageBox::No).exec())
    {
        QSqlQuery branco1(db.db);
        branco1.prepare("UPDATE votos SET votosBranco = votosBranco + 1 WHERE id = 'votos'");
        branco1.exec();

        QSqlQuery votou2(db.db);
        votou2.prepare("UPDATE usuario SET Votou = 'Sim'  WHERE Inscricao = (:inscricao)");
        votou2.bindValue(":inscricao", this->hashed);
        votou2.exec();

        ui1->winStack->setCurrentIndex(5);
        qInfo(logInfo()) << "Voto computado com sucesso";
        hash.gerarHash();
    }
}

void MainWindow::on_corrige_clicked()
{
    ui1->prefeito_1->setText("");
    ui1->prefeito_2->setText("");
    ui1->vereador_1->setText("");
    ui1->vereador_2->setText("");
    ui1->vereador_3->setText("");
    ui1->vereador_4->setText("");
    ui1->vereador_5->setText("");
    ui1->winStack->setCurrentIndex(1);
}

/* TELA 3 CONFIRMACAO */

void MainWindow::on_corrige_2_clicked()
{
    on_corrige_clicked();
}

void MainWindow::on_confirma_clicked()
{

    if(ui1->senha->text() == "") {

        ui1->senha->setPlaceholderText("Digite uma senha");

    }

    else {

        this->senha = ui1->senha->text();

        QSqlQuery updateVotoPeV(db.db);
        updateVotoPeV.prepare("UPDATE candidato SET votos = votos + 1 WHERE NumeroPartido = (:prefeito) OR NumeroPartido = (:vereador)");
        updateVotoPeV.bindValue(":prefeito", this->prefeito);
        updateVotoPeV.bindValue(":vereador", this->vereador);
        updateVotoPeV.exec();

        QSqlQuery voto(db.db);
        voto.prepare("UPDATE usuario SET Votou = 'Sim'  WHERE Inscricao = (:inscricao)");
        voto.bindValue(":inscricao", this->hashed);
        voto.exec();

        QSqlQuery votoencriptado(db.db);
        votoencriptado.prepare("INSERT INTO votoencriptado (titulohash, votoprefeito, votovereador) VALUES ((:inscricao1), AES_ENCRYPT((:prefeito1), (:senha)), AES_ENCRYPT((:vereador1), (:senha)))");
        votoencriptado.bindValue(":prefeito1", this->prefeito);
        votoencriptado.bindValue(":vereador1", this->vereador);
        votoencriptado.bindValue(":inscricao1", this->hashed);
        votoencriptado.bindValue(":senha", this->senha);
        votoencriptado.exec();

        ui1->winStack->setCurrentIndex(5);
        qInfo(logInfo()) << "Voto computado com sucesso";
        hash.gerarHash();

    }
}

void MainWindow::on_winStack_currentChanged(int index)
{
    if (index == 2 && this->candidatoIn)
    {

        QSqlQuery dadosP(db.db);
        dadosP.prepare("SELECT Nome,NumeroPartido,Imagens FROM candidato WHERE NumeroPartido = (:prefeito)");
        dadosP.bindValue(":prefeito", this->prefeito);
        dadosP.exec();

        QSqlQuery dadosV(db.db);
        dadosV.prepare("SELECT Nome,NumeroPartido,Imagens FROM candidato WHERE NumeroPartido = (:vereador)");
        dadosV.bindValue(":vereador", this->vereador);
        dadosV.exec();

        QString name_prefeito,numero_prefeito,name_vereador,numero_vereador;
        QByteArray outByteArray_prefeito,outByteArray_vereador;

        while (dadosP.next())
        {
            name_prefeito = dadosP.value(0).toString();
            numero_prefeito = dadosP.value(1).toString();
            outByteArray_prefeito = dadosP.value(2).toByteArray();
        }

        while (dadosV.next())
        {
            name_vereador = dadosV.value(0).toString();
            numero_vereador = dadosV.value(1).toString();
            outByteArray_vereador = dadosV.value(2).toByteArray();
        }

        QPixmap outPixmap_Prefeito = QPixmap();
        outPixmap_Prefeito.loadFromData(outByteArray_prefeito);

        QPixmap outPixmap_Vereador = QPixmap();
        outPixmap_Vereador.loadFromData(outByteArray_vereador);

        ui1->nome_p_query->setText(name_prefeito);
        ui1->numero_p_query->setText(numero_prefeito);
        ui1->receber_imagemP->setPixmap(outPixmap_Prefeito);

        ui1->nome_v_query->setText(name_vereador);
        ui1->numero_v_query->setText(numero_vereador);
        ui1->receber_imagemV->setPixmap(outPixmap_Vereador);
    }

    if (index == 5 && this->loggedIn)
    {
        QTimer::singleShot(2800,this,SLOT(logout()));
        audio.play();
    }
}

void MainWindow::logout()
{
    this->loggedIn = false;
    this->candidatoIn = false;
    ui1->usernameText->setText("");
    ui1->passwordText->setText("");
    ui1->prefeito_1->setText("");
    ui1->prefeito_2->setText("");
    ui1->vereador_1->setText("");
    ui1->vereador_2->setText("");
    ui1->vereador_3->setText("");
    ui1->vereador_4->setText("");
    ui1->vereador_5->setText("");
    ui1->senha->setText("");
    ui1->loginLabel->setText("Voto Realizado com Sucesso. Proximo?");
    ui1->winStack->setCurrentIndex(0);
    qInfo(logInfo()) << "Usuario Deslogado com Sucesso";
    hash.gerarHash();

}
/* TELA 4 ADMINISTRADOR */

void MainWindow::on_cadastroCandidato_clicked()
{
    ui1->admStack->setCurrentIndex(1);
}

void MainWindow::on_cadastrarCandidato_clicked()
{
    bool halt = false;

    if(ui1->nomeCandidato->text() == "")
    {
        ui1->nomeCandidato->setPlaceholderText("Nome do candidato vazio!");
        halt = true;
    }

    if(ui1->nomePartido->text() == "")
    {
        ui1->nomePartido->setPlaceholderText("Nome do partido em branco!");
        halt = true;
    }

    if(ui1->numeroPartido->text() == "")
    {
        ui1->numeroPartido->setPlaceholderText("Numero partidario em branco!");
        halt = true;
    }

    if(ui1->tipoCandidato->text() == "")
    {
        ui1->tipoCandidato->setPlaceholderText("Cargo em branco!");
        halt = true;
    }

    QSqlQuery cQuery(db.db);
    cQuery.prepare("SELECT Nome FROM candidato WHERE Nome = (:nome)");
    cQuery.bindValue(":nome", ui1->nomeCandidato->text());

    if(cQuery.exec())
    {
        if(cQuery.next())
        {
            ui1->nomeCandidato->setText("");
            ui1->nomeCandidato->setPlaceholderText("escolha outro nome!");
            halt = true;
        }
    }

    QSqlQuery cQuery2(db.db);
    cQuery2.prepare("SELECT NumeroPartido FROM candidato WHERE NumeroPartido = (:numeropartido)");
    cQuery2.bindValue(":numeropartido", ui1->numeroPartido->text());

    if(cQuery2.exec())
    {
        if(cQuery2.next())
        {
            ui1->numeroPartido->setText("");
            ui1->numeroPartido->setPlaceholderText("Use outro numero de partidario!");
            halt = true;
        }
    }

    if(halt)
    {
        ui1->regLabel->setText("Por favor corriga seus erros.");
    }

    else
    {
        QFile file (filename);
        if (!file.open(QIODevice::ReadOnly)) return;
        QByteArray byte = file.readAll();
        ui1->regLabel->setText("");

        QSqlQuery iQuery(db.db);
        iQuery.prepare("INSERT INTO candidato(Nome, Partido, NumeroPartido, Cargo, Imagens)"\
                       "VALUES(:nome, :partido, :numeropartido, :cargo, :imagens)");
        iQuery.bindValue(":nome", ui1->nomeCandidato->text());
        iQuery.bindValue(":partido", ui1->nomePartido->text());
        iQuery.bindValue(":numeropartido", ui1->numeroPartido->text());
        iQuery.bindValue(":cargo", ui1->tipoCandidato->text());
        iQuery.bindValue(":imagens", byte);

        if(iQuery.exec())
        {
            ui1->nomeCandidato->setText("");
            ui1->nomePartido->setText("");
            ui1->numeroPartido->setText("");
            ui1->tipoCandidato->setText("");
            ui1->enviar_imagem->clear();
            ui1->regLabel->setText("Candidato Registrado com sucesso!");
        }

    }
}

void MainWindow::on_upload_foto_clicked()
{
    this->filename = QFileDialog::getOpenFileName(this, tr("Choose"), "/", tr("*.png *.jpg"));
    ui1->enviar_imagem->setText("<img src=\"file:///"+this->filename+"\" alt=\"Erro de leitura da Imagem!\" height=\"128\" width=\"128\" />");
}



void MainWindow::on_zerarVotacao_clicked()
{
    ui1->admStack->setCurrentIndex(0);

    if(QMessageBox::Yes == QMessageBox(QMessageBox::Question,
                                       "Resetar", "Você tem certeza que deseja zerar a votação?",
                                       QMessageBox::Yes|QMessageBox::No).exec())
    {
        QSqlQuery resetvoto(db.db);
        resetvoto.prepare("UPDATE candidato SET Votos = 0");
        resetvoto.exec();

        QSqlQuery resetvotou(db.db);
        resetvotou.prepare("UPDATE usuario SET Votou = 'Não'");
        resetvotou.exec();

        QSqlQuery resetemqmvotou(db.db);
        resetemqmvotou.prepare("DELETE FROM votoencriptado");
        resetemqmvotou.exec();

        QFile file("logFile.log");
        file.open(QIODevice::ReadWrite | QIODevice::Truncate);
        QTextStream out(&file);
        out << "";

        hash.gerarHash();
    }

}

void MainWindow::on_logout_adm_clicked()
{
    if(QMessageBox::Yes == QMessageBox(QMessageBox::Question,
                                       "Sair", "Você tem certeza que deseja sair?",
                                       QMessageBox::Yes|QMessageBox::No).exec())
    {
        this->loggedIn = false;
        ui1->usernameText->setText("");
        ui1->passwordText->setText("");
        ui1->winStack->setCurrentIndex(0);
        qInfo(logInfo()) << "Administrador Deslogado com Sucesso";
        hash.gerarHash();
    }

}
/* TELA 5 AUDITOR */

void MainWindow::on_logEvent_clicked()
{
    ui1->auditorStack->setCurrentIndex(0);

    QFile file("logFile.log");
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    QByteArray fileData = file.readAll();
    QByteArray res = QCryptographicHash::hash(fileData, QCryptographicHash::Sha3_512).toHex();

    QSqlQuery hash_check(db.db);
    hash_check.prepare("SELECT hash_atual FROM hash WHERE hash_nome = 'log'");
    hash_check.exec();

    int id = hash_check.record().indexOf("hash_atual");

    QString hash;

    while (hash_check.next())
    {
        hash = hash_check.value(id).toString();
    }

    if (res == hash) {

        QMessageBox msgBox;
        msgBox.setText("O documento continua integro");
        msgBox.exec();

    }else {
        QMessageBox msgBox;
        msgBox.setText("O documento não está integro pode haver fraudes");
        msgBox.exec();
    }

//    QProcess *process = new QProcess(this);
//    QString file = QDir::homepath + "/logFile.exe";
//    process->start(file);


}

void MainWindow::on_apurarVotos_clicked()
{
    ui1->auditorStack->setCurrentIndex(1);
    QSqlQueryModel *model = new QSqlQueryModel(this);
    model->setQuery("SELECT Nome,Partido,NumeroPartido,Votos,Cargo FROM candidato");
    ui1->tableModel->setModel(model);

}

void MainWindow::on_recontagem_clicked()
{
    ui1->auditorStack->setCurrentIndex(2);
}

void MainWindow::on_eleitor_clicked()
{
    ui1->auditorStack->setCurrentIndex(3);
}

void MainWindow::on_decriptar_clicked()
{
    QString titulo = ui1->tituloeleitor->text();

    QByteArray titulogeneratehash = titulo.toUtf8();
    QByteArray titulohash = QCryptographicHash::hash(titulogeneratehash, QCryptographicHash::Sha3_512).toHex();

    QSqlQuery eleitor1(db.db);
    eleitor1.prepare("SELECT CONVERT((AES_DECRYPT(votoprefeito,:senha1)) USING utf8) AS votouemprefeito, CONVERT((AES_DECRYPT(votovereador,:senha1)) USING utf8) AS votouemvereador FROM votoencriptado WHERE titulohash =(:titulohash)");
    eleitor1.bindValue(":titulohash", titulohash);
    eleitor1.bindValue(":senha1", ui1->senha_decrypt->text());
    eleitor1.exec();

    QString votouemprefeito;
    QString votouemvereador;

    while (eleitor1.next())
    {
        votouemprefeito = eleitor1.value(0).toString();
        votouemvereador = eleitor1.value(1).toString();
    }
    ui1->votouem->setText("votou no Prefeito Numero: " + votouemprefeito + " E No Vereador Numero: " + votouemvereador);
}

void MainWindow::on_logout_aud_clicked()
{
    if(QMessageBox::Yes == QMessageBox(QMessageBox::Question,
                                       "Sair", "Você tem certeza que deseja sair?",
                                       QMessageBox::Yes|QMessageBox::No).exec())
    {
        this->loggedIn = false;
        ui1->usernameText->setText("");
        ui1->passwordText->setText("");
        ui1->winStack->setCurrentIndex(0);
        //qInfo(logInfo()) << "Auditor Deslogado com Sucesso";
        //gerarHash();
    }
}
