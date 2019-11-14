#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    server = new chatServer();
    connect(server, &chatServer::signal_serverError, this, &MainWindow::netError);
    connect(server, &chatServer::signal_serverStarted, this, &MainWindow::netSuccess);
    connect(server, &chatServer::signal_databaseOpen, this, &MainWindow::baseSuccess);
    connect(server, &chatServer::signal_serverError, this, &MainWindow::baseError);
    connect(ui->pbStartServer, &QPushButton::clicked, server, &chatServer::serverStart);
    //connect(ui->pushB, & );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::netError(const QString &text)
{
    ui->pbStartServer->setEnabled(true);
    ui->teStatus->insertPlainText("Ошибка подключения к серверу: " + text + "\n");
}

void MainWindow::netSuccess()
{
    ui->pbStartServer->setEnabled(false);
    ui->teStatus->insertPlainText("Сервер успешно запущен\n");
}

void MainWindow::baseSuccess()
{
    ui->teStatus->insertPlainText("Успешное подключение к СУБД\n");
}

void MainWindow::baseError(const QString &text)
{
    ui->teStatus->insertPlainText("Ошибка подкючения к СУБД: " + text + "\n");
}


void MainWindow::on_actionExit_triggered()
{
    MainWindow::close();
}



void MainWindow::on_actionNew_connect_triggered()
{
    choisePort * choise = new choisePort();
    choise->show();

}
