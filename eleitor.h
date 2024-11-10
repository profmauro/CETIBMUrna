#ifndef ELEITOR_H
#define ELEITOR_H

#include <QWidget>

namespace Ui {
class Eleitor;
}

class Eleitor : public QWidget
{
    Q_OBJECT

public:
    explicit Eleitor(QWidget *parent = nullptr);
    ~Eleitor();

protected:
    void closeEvent(QCloseEvent *event) override;

public slots:
    void processo_voto(const QString &processo, bool terminou);

    void recebendo_eleitor(const QString &nome);

signals:
    void enviar_matricula(const QString &matricula);

    void iniciar_votacao();

private slots:
    void on_button_liberar_clicked();

    void on_pushButton_clicked();

private:
    Ui::Eleitor *ui;
    QString correctPassword = "somenteeumace";
};

#endif // ELEITOR_H
