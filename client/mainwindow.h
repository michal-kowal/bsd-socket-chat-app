#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <QTcpSocket>
#include <QByteArray>
#include <QListWidget>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QString>
#include <QWidget>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include "structures.h"
#include "dataHandler.h"

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

protected:
    QTcpSocket * socket {nullptr};
    bool loginExists;
    bool password;
    int decide; //0-signup 1-login
    bool status;
    Chat* activeChat;
    std::vector<QString> users;
    std::vector<Chat*> activeChats;

    void connection();
    void changeLoginSection(bool val);
    void socketConnected();
    void receivePacket();
    void checkLoginUser();
    void chooseProcedure();
    void signUpProcedure();
    void logInProcedure();
    void signUpUser();
    void loginUser();
    void logOutUser();
    void sendLogin(QString login);
    void sendPassword(QString password);
    void sendLoginToLogin(QString login);
    void sendPasswordToLogin(QString password);
    void onUserItemDoubleClicked(QListWidgetItem *item);
    void requestUsersList();
    void displayUsersList();
    void refreshUsersList();
    void newConnectionDisplay(QString data);
    void ack(QString data, enum packetType type);
    void displayActiveChats();

    void displayMessages(QListWidgetItem *item);
    void presentMessages(std::vector<Message> mess);
    void receiveMessage(const char* mess);

    void deleteChat();
    void performDelete(QString name, int check);

    void sendMessage();
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
