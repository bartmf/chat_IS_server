#ifndef CHOISEPORT_H
#define CHOISEPORT_H

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class choisePort;
}

class choisePort : public QDialog
{
    Q_OBJECT

public:
    explicit choisePort(QWidget *parent = nullptr);
    ~choisePort();



private slots:
    void on_pushButtonCancel_clicked();

    void on_pushButtonOk_clicked();

signals:
    void signalToChaingePort(quint16);
private:
    Ui::choisePort *ui;
};

#endif // CHOISEPORT_H
