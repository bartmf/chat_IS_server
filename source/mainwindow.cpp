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
    connect(ui->pbStartServer, &QPushButton::clicked, server, &chatServer::serverStart);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::netError(const QString &text)
{
    ui->pbStartServer->setEnabled(true);
    ui->teStatus->insertPlainText(text + "\n");
}

void MainWindow::netSuccess()
{
    ui->pbStartServer->setEnabled(false);
    ui->teStatus->insertPlainText("Сервер успешно запущен\n");
}
