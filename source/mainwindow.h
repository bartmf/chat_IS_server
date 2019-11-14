#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "chatserver.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "choiseport.h"
#include "commandline.h"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionExit_triggered();

    void on_actionNew_connect_triggered();

private:
    Ui::MainWindow *ui;
    chatServer* server;
    CommandLine * commandLine;

private:
    void netError(const QString& text);
    void netSuccess();
    void auth(const QJsonDocument& doc);
    void registration (const QJsonDocument& doc);
    void baseSuccess();
    void baseError(const QString& text);

};
#endif // MAINWINDOW_H
