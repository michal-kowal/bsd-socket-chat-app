#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::logOutUser(){
    for(auto &chat: activeChats){
        performDelete(QString(chat->receiver), 1);
    }
    Packet packet;
    packet.type = P_LOGOUT_REQUEST;
    packet.size = 0;
    sendData(*socket, packet);
}

void MainWindow::checkLoginUser(){
    Packet packet;
    QString login = ui->loginLineEdit->text();
    packet.type = P_ASK_LOGIN_USER;
    packet.size = sizeof(login.length());
    packet.data = const_cast<char*>(login.toStdString().c_str());
    sendData(*socket, packet);
}

void MainWindow::sendLogin(QString login){
    Packet packet;
    packet.type = P_SEND_LOGIN;
    packet.size = sizeof(login.length());
    packet.data = const_cast<char*>(login.toStdString().c_str());
    sendData(*socket, packet);
}

void MainWindow::sendPassword(QString password){
    Packet packet;
    packet.type = P_SEND_PASSWORD;
    packet.size = sizeof(password.length());
    packet.data = const_cast<char*>(password.toStdString().c_str());
    sendData(*socket, packet);
}

void MainWindow::signUpUser(){
    checkLoginUser();
    decide = 0;
}

void MainWindow::sendLoginToLogin(QString login){
    Packet packet;
    packet.type = P_SEND_LOGIN_LOG;
    packet.size = sizeof(login.length());
    packet.data = const_cast<char*>(login.toStdString().c_str());
    sendData(*socket, packet);
}

void MainWindow::sendPasswordToLogin(QString password){
    Packet packet;
    packet.type = P_SEND_PASSWORD_LOG;
    packet.size = sizeof(password.length());
    packet.data = const_cast<char*>(password.toStdString().c_str());
    sendData(*socket, packet);
}

void MainWindow::loginUser(){
    checkLoginUser();
    decide = 1;
}

void MainWindow::logInProcedure(){
    QString login = ui->loginLineEdit->text();
    QString password = ui->passwordLineEdit->text();
    if(login.length()!=0 && password.length()!=0 && loginExists){
        sendLoginToLogin(login);
        sendPasswordToLogin(password);
    }else if (!loginExists){
        ui->infoBox->setText("User doesn't exist.");
    }
}

void MainWindow::signUpProcedure(){
    QString login = ui->loginLineEdit->text();
    QString password = ui->passwordLineEdit->text();
    if(login.length()!=0 && password.length()!=0 && !loginExists){
        sendLogin(login);
        sendPassword(password);
    }else if (loginExists){
        ui->infoBox->setText("User already exists.");
    }
}

void MainWindow::chooseProcedure(){
    if(decide==0) signUpProcedure();
    else if(decide==1) logInProcedure();
}

void MainWindow::changeLoginSection(bool val){
    ui->loginLineEdit->setEnabled(val);
    ui->passwordLineEdit->setEnabled(val);
    ui->logInButton->setEnabled(val);
    ui->signUpButton->setEnabled(val);
    ui->logoutButton->setEnabled(!val);
    ui->refreshButton->setEnabled(!val);
    ui->refreshButton->setVisible(!val);
    ui->UsersOnline->setVisible(!val);
    ui->ActiveChatName->setVisible(!val);
    ui->ChatWith->setVisible(!val);
    ui->messageTextEdit->setVisible(!val);
    ui->sendButton->setVisible(!val);
    ui->closeChatButton->setVisible(!val);
    ui->chatTextBrowser->setVisible(!val);
    QHBoxLayout* layout = ui->usersList;
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
    QHBoxLayout* layout1 = ui->activeChats;
    QLayoutItem *child1;
    while ((child1 = layout1->takeAt(0)) != nullptr) {
        delete child1->widget();
        delete child1;
    }
}
