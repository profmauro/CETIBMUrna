#include "eleitor.h"
#include "ui_eleitor.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QCloseEvent>

Eleitor::Eleitor(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Eleitor)
{
    setWindowTitle("Eleitores");
    ui->setupUi(this);
    ui->groupBox->setVisible(false);
    ui->label_nome->setVisible(false);
}

Eleitor::~Eleitor()
{
    delete ui;
}

void Eleitor::closeEvent(QCloseEvent *event) {
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

void Eleitor::on_button_liberar_clicked()
{
    emit iniciar_votacao();
    ui->frame->setVisible(false);
    ui->groupBox->setVisible(true);
}

void Eleitor::processo_voto(const QString &processo, bool terminou){
    ui->label_processo_votacao->setText(processo);
    if(terminou){
        ui->frame->setVisible(true);
        ui->groupBox->setVisible(false);
        ui->lineEdit_matricula->setText("");
        ui->label_nome->setVisible(false);
        ui->label_nome_eleitor->setText("");
        ui->button_liberar->setEnabled(false);
    }
}

void Eleitor::recebendo_eleitor(const QString &nome){
    if((!nome.isEmpty())){
        ui->label_nome_eleitor->setText(nome);
        ui->button_liberar->setEnabled(true);
        ui->label_nome->setVisible(true);
    }else{
        ui->button_liberar->setEnabled(false);
        ui->label_nome->setVisible(false);
        ui->label_nome_eleitor->setText("Matrícula não encontrada");
    }

}

void Eleitor::on_pushButton_clicked()
{
    emit enviar_matricula(ui->lineEdit_matricula->text());
}

