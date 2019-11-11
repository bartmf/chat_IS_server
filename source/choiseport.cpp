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
