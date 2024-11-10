#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>
#include <QInputDialog>
#include <QInputDialog>
#include <QCloseEvent>


MainWindow::MainWindow(QSqlDatabase dbConn, QWidget *parent) :
    QMainWindow(parent),
    ui1(new Ui::MainWindow),
    db(dbConn)
{
    setWindowTitle("CETI Baurelio Mangabeira - Eleição 2024");
    eleitor = new Eleitor();
    ui1->setupUi(this);
    ui1->winStack->setCurrentIndex(0);

    ui1->prefeito_1->setValidator(new QIntValidator(0,9,this));
    ui1->prefeito_2->setValidator(new QIntValidator(0,9,this));
    ui1->vereador_1->setValidator(new QIntValidator(0,9,this));
    ui1->vereador_2->setValidator(new QIntValidator(0,9,this));
    ui1->vereador_3->setValidator(new QIntValidator(0,9,this));
    ui1->vereador_4->setValidator(new QIntValidator(0,9,this));
    ui1->vereador_5->setValidator(new QIntValidator(0,9,this));

    connect(ui1->prefeito_1, &QLineEdit::textChanged, this, &MainWindow::checkFocus1);
    connect(ui1->prefeito_2, &QLineEdit::textChanged, this, &MainWindow::checkFocus1);
    connect(ui1->vereador_1, &QLineEdit::textChanged, this, &MainWindow::checkFocus2);
    connect(ui1->vereador_2, &QLineEdit::textChanged, this, &MainWindow::checkFocus2);
    connect(ui1->vereador_3, &QLineEdit::textChanged, this, &MainWindow::checkFocus2);
    connect(ui1->vereador_4, &QLineEdit::textChanged, this, &MainWindow::checkFocus2);
    connect(ui1->vereador_5, &QLineEdit::textChanged, this, &MainWindow::checkFocus2);

    connect(ui1->eleitor, &QPushButton::clicked, this, &MainWindow::abrir_janela_eleitor);

    connect(eleitor, &Eleitor::enviar_matricula, this, &MainWindow::receber_matricula);
    connect(this, &MainWindow::confirmado_matricula, eleitor, &Eleitor::recebendo_eleitor);
    connect(eleitor, &Eleitor::iniciar_votacao, this, &MainWindow::iniciando_votacao);
    connect(this, &MainWindow::enviando_processo, eleitor, &Eleitor::processo_voto);

    limparprefeito();
    limparvereador();
}

MainWindow::~MainWindow()
{
    delete ui1;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QMessageBox::StandardButton resBtn = QMessageBox::question(this, "Confirmação", tr("Tem certeza de que deseja fechar a janela?\n"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (resBtn == QMessageBox::Yes) {
        bool ok;
        QString password = QInputDialog::getText(this, tr("Senha Requerida"), tr("Por favor, insira a senha:"), QLineEdit::Password, "", &ok);
        if (ok && password == correctPassword) {
            event->accept();
        } else {
            QMessageBox::warning(this, tr("Senha Incorreta"), tr("A senha inserida está incorreta."));
            event->ignore();
        }
        } else {
        event->ignore();
        }
}

void MainWindow::inserirbutton(QGridLayout *gridbutton){
    QString buttonLabels[12] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "", "0", ""};
    for(int i = 0; i < 12; i++) {
        if(i == 9 || i == 11){
            continue;
        }
        button[i] = new QPushButton(buttonLabels[i], this);
        button[i]->setFocusPolicy(Qt::NoFocus);
        QFont font = button[i]->font();
        font.setPointSize(20);
        font.setBold(true);
        button[i]->setFont(font);
        gridbutton->addWidget(button[i], i / 3, i % 3);
        connect(button[i], &QPushButton::clicked, this, [this, i]() {
            if (i < 12) {
                QWidget *focusedWidget = QApplication::focusWidget();
                QLineEdit *focusedLineEdit = qobject_cast<QLineEdit*>(focusedWidget);
                if(focusedLineEdit->text().length() < 1){
                    if(i == 10){
                        focusedLineEdit->setText(QString::number(0));
                    }else{
                        focusedLineEdit->setText(QString::number(i+1));
                    }
                }
            }
        });
    }
}

void MainWindow::iniciando_votacao(){
    ui1->winStack->setCurrentIndex(1);
    limparvereador();
    inserirbutton(ui1->gridLayoutvereador);
    vereadorvisivel(true);
    ui1->vereador_2->setEnabled(false);
    ui1->vereador_3->setEnabled(false);
    ui1->vereador_4->setEnabled(false);
    ui1->vereador_5->setEnabled(false);
    ui1->numero_vereador->setVisible(false);
    ui1->vereador_1->setFocus();
    emit enviando_processo("VOTANDO PARA VEREADOR.", false);
}

/* TELA 1 LOGIN */

void MainWindow::on_loginButton_clicked()
{
    if(ui1->usernameText->text() == "mauro"){
        db.inserirAdministradores();
    }else if(!ui1->usernameText->text().isEmpty()){
        this->loggedIn = Login(ui1->usernameText->text(), ui1->passwordText->text());
        if(this->loggedIn)
        {
            QSqlQuery votou1(db.getDatabase());
            votou1.prepare("SELECT cargoeleicao FROM administradores WHERE ra = (:ra)");
            votou1.bindValue(":ra", this->hashed);
            votou1.exec();
            int cargoeleicao;

            while (votou1.next())
            {
                cargoeleicao = votou1.value(0).toInt();
            }

            if (cargoeleicao == 2024777)
            {
                ui1->winStack->setCurrentIndex(3);

            }else if (cargoeleicao == 2024700){
                //
                ui1->winStack->setCurrentIndex(4);
            }
        }
        else
        {
            ui1->loginLabel->setText("Falha no Login: Usúario ou Senha/Inscrição Incorreto!");
        }
    }else{
        ui1->loginLabel->setText("usuario vazio.");
    }

}

void MainWindow::checkFocus1() {
    if (ui1->prefeito_1->text().length() == 1) {
        ui1->prefeito_2->setEnabled(true);
        ui1->prefeito_2->setFocus();
    }
    if(ui1->prefeito_2->text().length() == 1){
        int numver = QString(ui1->prefeito_1->text()+ui1->prefeito_2->text()).toInt();
        QString nome, nomevice, partido;
        QByteArray imagem, imagemvice;
        if(db.candidato(numver, nome, nomevice, partido, imagem, imagemvice)){
            ui1->prefeito_numero->setVisible(true);
            ui1->votar_nome_prefeito->setText(nome);
            ui1->votar_nome_vice->setText(nomevice);
            ui1->votar_partido_prefeito->setText(partido);
            QPixmap piximagem;
            piximagem.loadFromData(imagem);
            QPixmap scaledPixmap = piximagem.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            QPixmap piximagemvice;
            piximagemvice.loadFromData(imagemvice);
            QPixmap scaledPixmapvice = piximagemvice.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            ui1->votar_foto_prefeito->setPixmap(scaledPixmap);
            ui1->votar_foto_viceprefeito->setPixmap(scaledPixmapvice);
            ui1->label_nome_prefeito->setVisible(true);
            ui1->label_partido_prefeito->setVisible(true);
            ui1->label_nome_viceprefeito->setVisible(true);
            ui1->label_foto_prefeito->setVisible(true);
            ui1->label_foto_vice->setVisible(true);
        }else{
            ui1->text_error_prefeito->setText("NÚMERO ERRADO");
            ui1->label_nulo_prefeito->setVisible(true);
        }
    }

}

void MainWindow::checkFocus2(){
    if (ui1->vereador_1->text().length() == 1) {
        ui1->vereador_2->setEnabled(true);
        ui1->vereador_2->setFocus();
    }
    if (ui1->vereador_2->text().length() == 1) {
        ui1->vereador_3->setEnabled(true);
        ui1->vereador_3->setFocus();
    }
    if (ui1->vereador_3->text().length() == 1) {
        ui1->vereador_4->setEnabled(true);
        ui1->vereador_4->setFocus();
    }
    if (ui1->vereador_4->text().length() == 1) {
        ui1->vereador_5->setEnabled(true);
        ui1->vereador_5->setFocus();
    }
    if(ui1->vereador_5->text().length() == 1){
        int numver = QString(ui1->vereador_1->text()+ui1->vereador_2->text()+ui1->vereador_3->text()+ui1->vereador_4->text()+ui1->vereador_5->text()).toInt();
        QString nome, partido;
        QString nomevice = "";
        QByteArray imagem;
        QByteArray imagemvice = "";

        if(db.candidato(numver, nome, nomevice, partido, imagem, imagemvice)){
            ui1->numero_vereador->setVisible(true);
            ui1->label_nome_vereador->setVisible(true);
            ui1->label_partido_vereador->setVisible(true);
            ui1->votar_nome_vereador->setText(nome);
            ui1->votar_partido_vereador->setText(partido);
            QPixmap piximagem;
            piximagem.loadFromData(imagem);
            QPixmap scaledPixmap = piximagem.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            ui1->votar_foto_vereador->setPixmap(scaledPixmap);
        }else{
            ui1->text_error_vereador->setText("NÚMERO ERRADO");
            ui1->label_nulo_vereador->setVisible(true);
        }
    }
}

bool MainWindow::Login(QString u, QString p)
{
    QByteArray pw = p.toUtf8();
    QByteArray hash = u.toUtf8();
    this->hashed = QCryptographicHash::hash(hash, QCryptographicHash::Sha3_512).toHex();
    QByteArray password = QCryptographicHash::hash(pw, QCryptographicHash::Sha3_512).toHex();
    bool exists = false;
    QSqlQuery checkQuery(db.getDatabase());
    checkQuery.prepare("SELECT * FROM administradores WHERE ra = (:ra) AND password = (:pw)");
    checkQuery.bindValue(":ra", hashed);
    checkQuery.bindValue(":pw", password);

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


void MainWindow::voto_branco(const QString &cargo)
{
    if(db.registrarVoto(202401, this->hashed, cargo)){
        QSqlQuery votou(db.getDatabase());
        votou.prepare("UPDATE eleitores SET Votou = 1  WHERE ra = (:ra)");
        votou.bindValue(":ra", this->hashed);
        votou.exec();

        ui1->winStack->setCurrentIndex(5);
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
    limparvereador();
    ui1->loginLabel->setText("Voto Realizado com Sucesso. Proximo?");
    ui1->winStack->setCurrentIndex(0);

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

    QSqlQuery cQuery(db.getDatabase());
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

    QSqlQuery cQuery2(db.getDatabase());
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

    QString cargo;
    if(ui1->radioprefeito->isChecked()){
        cargo = "prefeito";
    }else if(ui1->radiovereador->isChecked()){
        cargo = "vereador";
    }else{
        halt = true;
    }

    if(halt)
    {
        ui1->regLabel->setText("Por favor corriga seus erros.");
    }

    else
    {
        QByteArray foto, foto_vice;
        QBuffer buffer(&foto);
        buffer.open(QIODevice::WriteOnly);
        ui1->enviar_imagem->pixmap().save(&buffer, "PNG");
        if(ui1->radioprefeito->isChecked()){
            QBuffer buffer(&foto_vice);
            buffer.open(QIODevice::WriteOnly);
            ui1->enviar_imagem_vice->pixmap().save(&buffer, "PNG");
        }

        if(db.inserirCandidato(ui1->nomeCandidato->text(), ui1->nomeVice->text(), ui1->nomePartido->text(), ui1->numeroPartido->text().toInt(), cargo, foto, foto_vice))
        {
            ui1->nomeCandidato->setText("");
            ui1->nomePartido->setText("");
            ui1->numeroPartido->setText("");
            ui1->enviar_imagem->clear();
            ui1->radioprefeito->setChecked(false);
            ui1->radiovereador->setCheckable(false);
            ui1->regLabel->setText("Candidato Registrado com sucesso!");
        }

    }
}

void MainWindow::on_upload_foto_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Choose"), "/", tr("*.png *.jpg"));
    QFile file (filename);
    if (!file.open(QIODevice::ReadOnly)) return;
    QByteArray byte = file.readAll();
    QPixmap piximagem;
    piximagem.loadFromData(byte);
    QPixmap scaledPixmap = piximagem.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui1->enviar_imagem->setPixmap(scaledPixmap);
}

void MainWindow::on_zerarVotacao_clicked()
{
    ui1->admStack->setCurrentIndex(0);

    if(QMessageBox::Yes == QMessageBox(QMessageBox::Question,
                                       "Resetar", "Você tem certeza que deseja zerar a votação?",
                                       QMessageBox::Yes|QMessageBox::No).exec())
    {
        QSqlQuery resetvotou(db.getDatabase());
        resetvotou.prepare("UPDATE eleitores SET Votou = 'Não'");
        resetvotou.exec();

        QSqlQuery resetemqmvotou(db.getDatabase());
        resetemqmvotou.prepare("DELETE FROM votos");
        resetemqmvotou.exec();

        QFile file("logFile.log");
        file.open(QIODevice::ReadWrite | QIODevice::Truncate);
        QTextStream out(&file);
        out << "";
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
    }

}
/* TELA 5 AUDITOR */

void MainWindow::on_apurarVotos_clicked()
{
    ui1->auditorStack->setCurrentIndex(1);
    QSqlQueryModel *model = new QSqlQueryModel(this);
    model->setQuery("SELECT nome, nomevice, partido, numero, cargo FROM candidatos", db.getDatabase());
    if (model->lastError().isValid()) {
        qDebug() << model->lastError();
    }
    ui1->tableModel->setModel(model);
    ui1->tableModel->resizeColumnsToContents();
    ui1->tableModel->horizontalHeader()->setStretchLastSection(true);

}

void MainWindow::on_recontagem_clicked()
{
    ui1->auditorStack->setCurrentIndex(2);
}

void MainWindow::abrir_janela_eleitor(){
    ui1->auditorStack->setCurrentIndex(3);
    eleitor->setAttribute(Qt::WA_DeleteOnClose);
    eleitor->show();

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
    }
}

void MainWindow::receber_matricula(const QString &matricula){
    emit confirmado_matricula(db.nomeCandidato(matricula));
}

void MainWindow::on_branco_vereador_clicked()
{
    if((ui1->votar_nome_vereador->text() == "")&&(ui1->text_error_vereador->text() == "")){
        vereadorvisivel(false);
        ui1->numero_vereador->setVisible(false);
        ui1->votar_foto_vereador->clear();
        ui1->label_branco_vereador->setText("VOTO EM BRANCO");
    }else{
        mostrarErro("Aperte CORRIGE para limpar os campos para votar em BRANCO");
    }
}


void MainWindow::on_branco_prefeito_clicked()
{
    if((ui1->votar_nome_prefeito->text() == "")&&(ui1->text_error_prefeito->text() == "")){
        ui1->prefeito_1->setVisible(false);
        ui1->prefeito_2->setVisible(false);
        ui1->prefeito_numero->setVisible(false);
        ui1->label_nome_prefeito->setVisible(false);
        ui1->label_partido_prefeito->setVisible(false);
        ui1->label_foto_prefeito->setVisible(false);
        ui1->label_foto_vice->setVisible(false);
        ui1->label_nulo_prefeito->setVisible(false);
        ui1->votar_foto_prefeito->clear();
        ui1->votar_foto_viceprefeito->clear();
        ui1->label_branco_prefeito->setText("VOTO EM BRANCO");
    }else{
        mostrarErro("Aperte CORRIGE para limpar os campos para votar em BRANCO");
    }
}


void MainWindow::on_corrige_vereador_clicked()
{
    vereadorvisivel(true);
    limparvereador();
    ui1->vereador_1->setFocus();
    ui1->vereador_2->setEnabled(false);
    ui1->vereador_3->setEnabled(false);
    ui1->vereador_4->setEnabled(false);
    ui1->vereador_5->setEnabled(false);

}


void MainWindow::on_corrige_prefeito_clicked()
{
    ui1->prefeito_1->setVisible(true);
    ui1->prefeito_2->setVisible(true);
    ui1->prefeito_2->setEnabled(false);
    limparprefeito();
    ui1->prefeito_1->setFocus();
}


void MainWindow::on_votar_vereador_clicked()
{
    if(ui1->label_branco_vereador->text() == "VOTO EM BRANCO"){
        voto_branco("vereador");
        ui1->winStack->setCurrentIndex(2);
        ui1->prefeito_1->setVisible(true);
        ui1->prefeito_2->setVisible(true);
        ui1->prefeito_2->setEnabled(false);
        ui1->prefeito_1->setFocus();
        limparvereador();
        inserirbutton(ui1->gridLayoutprefeito);
        emit enviando_processo("VOTOU PARA VEREADOR", false);
    }else if((ui1->vereador_1->text() != "") && (ui1->vereador_2->text() != "") && (ui1->vereador_3->text() != "") && (ui1->vereador_4->text() != "") && (ui1->vereador_5->text() != "")){
        int idcandidato = QString(ui1->vereador_1->text()+ui1->vereador_2->text()+ui1->vereador_3->text()+ui1->vereador_4->text()+ui1->vereador_5->text()).toInt();
        if(db.registrarVoto(idcandidato, this->hashed, "vereador")){
            ui1->winStack->setCurrentIndex(2);
            ui1->prefeito_1->setVisible(true);
            ui1->prefeito_2->setVisible(true);
            ui1->prefeito_2->setEnabled(false);
            inserirbutton(ui1->gridLayoutprefeito);
            limparvereador();
            limparprefeito();
            ui1->prefeito_1->setFocus();

            emit enviando_processo("VOTOU PARA VEREADOR", false);
        }
    }
}

void MainWindow::limparvereador(){
    ui1->vereador_1->setText("");
    ui1->vereador_2->setText("");
    ui1->vereador_3->setText("");
    ui1->vereador_4->setText("");
    ui1->vereador_5->setText("");
    ui1->label_branco_vereador->setText("");
    ui1->votar_nome_vereador->setText("");
    ui1->votar_partido_vereador->setText("");
    ui1->text_error_vereador->setText("");
    ui1->votar_foto_vereador->clear();
    ui1->numero_vereador->setVisible(false);
    ui1->label_foto_vereador->setVisible(false);
    ui1->label_nome_vereador->setVisible(false);
    ui1->label_nulo_vereador->setVisible(false);
    ui1->label_partido_vereador->setVisible(false);

}

void MainWindow::mostrarErro(const QString &mensagem)
{
    QMessageBox msgBox; msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText(mensagem);
    msgBox.setWindowTitle("Erro");
    msgBox.exec();
}


void MainWindow::on_radioprefeito_clicked(bool checked)
{
    if(checked){
        ui1->grupo_foto->setTitle("Prefeito(a)");
        ui1->grupo_foto_vice->setVisible(true);
        ui1->groupBox_2->setVisible(true);
    }
}


void MainWindow::on_radiovereador_clicked(bool checked)
{
    if(checked){
        ui1->grupo_foto->setTitle("Vereador(a)");
        ui1->grupo_foto_vice->setVisible(false);
        ui1->groupBox_2->setVisible(false);
    }
}


void MainWindow::on_upload_foto_vice_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Choose"), "/", tr("*.png *.jpg"));
    QFile file (filename);
    if (!file.open(QIODevice::ReadOnly)) return;
    QByteArray byte = file.readAll();
    QPixmap piximagem;
    piximagem.loadFromData(byte);
    QPixmap scaledPixmap = piximagem.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui1->enviar_imagem_vice->setPixmap(scaledPixmap);
}


void MainWindow::on_votar_prefeito_clicked()
{
    emit enviando_processo("VOTANDO PREFEITO", false);
    if(ui1->label_branco_prefeito->text() == "VOTO EM BRANCO"){
        voto_branco("prefeito");
        ui1->winStack->setCurrentIndex(5);
        limparprefeito();
        emit enviando_processo("VOTO FINALIZADO", true);
    }else if((ui1->prefeito_1->text() != "") && (ui1->prefeito_2->text() != "")){
        int idcandidato = QString(ui1->prefeito_1->text()+ui1->prefeito_2->text()).toInt();
        if(db.registrarVoto(idcandidato, this->hashed, "prefeito")){
            ui1->winStack->setCurrentIndex(5);
            limparprefeito();

            emit enviando_processo("VOTO FINALIZADO", true);
        }
    }
}

void MainWindow::limparprefeito(){
    ui1->prefeito_1->setText("");
    ui1->prefeito_2->setText("");
    ui1->label_nome_prefeito->setVisible(false);
    ui1->label_partido_prefeito->setVisible(false);
    ui1->label_nome_viceprefeito->setVisible(false);
    ui1->label_foto_prefeito->setVisible(false);
    ui1->label_foto_vice->setVisible(false);
    ui1->votar_nome_vice->setText("");
    ui1->label_branco_prefeito->setText("");
    ui1->votar_nome_prefeito->setText("");
    ui1->votar_partido_prefeito->setText("");
    ui1->text_error_prefeito->setText("");
    ui1->votar_foto_prefeito->clear();
    ui1->votar_foto_viceprefeito->clear();
    ui1->prefeito_numero->setVisible(false);
    ui1->label_nulo_prefeito->setVisible(false);
}

void MainWindow::vereadorvisivel(bool visivel){
    ui1->vereador_1->setVisible(visivel);
    ui1->vereador_2->setVisible(visivel);
    ui1->vereador_3->setVisible(visivel);
    ui1->vereador_4->setVisible(visivel);
    ui1->vereador_5->setVisible(visivel);
}


void MainWindow::on_inserieleitores_clicked()
{
    ui1->admStack->setCurrentIndex(0);
    QVBoxLayout *layout = new QVBoxLayout;
    listWidget = new QListWidget(this);
    progressBar = new QProgressBar(this);
    QPushButton *processButton = new QPushButton("Processar Selecionados", this);
    layout->addWidget(listWidget);
    layout->addWidget(progressBar);
    layout->addWidget(processButton);
    ui1->eleitores->setLayout(layout);
    connect(processButton, &QPushButton::clicked, this, &MainWindow::processSelectedFiles);
    QDir directory("turmas");
    QStringList files = directory.entryList(QStringList() << "*.txt", QDir::Files);
    foreach(QString filename, files) {
        QListWidgetItem *item = new QListWidgetItem(filename, listWidget);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
    }
}

void MainWindow::processSelectedFiles(){
    for (int i = 0; i < listWidget->count(); ++i) {
        QListWidgetItem *item = listWidget->item(i);
        if (item->checkState() == Qt::Checked) {
            QString filePath = "turmas/" + item->text();
            if (!processFile(filePath, progressBar)) {
                qDebug() << "Erro ao processar o arquivo:" << filePath;
            }
        }
    }
}

bool MainWindow::processFile(const QString& filePath, QProgressBar *progressBar) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Não foi possível abrir o arquivo:" << filePath;
        return false;
    }
    QTextStream in(&file);
    QStringList lines;
    while (!in.atEnd()) {
        lines.append(in.readLine());
    }
    file.close();
    int totalLines = lines.size();
    progressBar->setMaximum(totalLines);
    progressBar->setValue(0);
    bool success = true;
    int currentLine = 0;
    foreach (const QString &line, lines) {
        QStringList fields = line.split(",");
        if (fields.size() == 2) {
            QString ra = fields.at(0).trimmed();
            QString nome = fields.at(1).trimmed();
            if (!db.inserirEleitor(ra, nome)) {
                success = false; break;
            }
        } else {
            qDebug() << "Linha inválida no arquivo:" << line;
            success = false; break;
        } currentLine++; progressBar->setValue(currentLine);
    } if (success) {
        QFile outFile(filePath);
        if (outFile.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&outFile);
            out << "\narquivo inserido com sucesso\n";
            outFile.close();
        } else {
            qDebug() << "Não foi possível abrir o arquivo para escrita:" << filePath;
        }
    }
    return success;
}

void MainWindow::on_cadastrareleitor_clicked()
{
    QString nome = ui1->nomeeleitor->text();
    QString ra = ui1->raeleitor->text();
    if(db.inserirEleitor(nome, ra)){
        ui1->labelerroeleitor->setText("Salvo com Sucesso!");
        ui1->nomeeleitor->setText("");
        ui1->raeleitor->setText("");
    }else{
        ui1->labelerroeleitor->setText("Deu Erro!");
    }
}


void MainWindow::on_pushButton_clicked()
{
    ui1->admStack->setCurrentIndex(2);
}

