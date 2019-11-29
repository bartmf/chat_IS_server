#include "choiseport.h"
#include "ui_choiseport.h"

choisePort::choisePort(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::choisePort)
{
    ui->setupUi(this);
}

choisePort::~choisePort()
{
    delete ui;
}



void choisePort::on_pushButtonCancel_clicked()
{
    choisePort::close();
}

void choisePort::on_pushButtonOk_clicked()
{
    QString port = ui->lineEditPort->text();
    if(port.toInt() > 0){
        quint16 port16 = port.toInt();
        emit signalToChaingePort(port16);
        choisePort::close();
    }
    else{

    }
}
