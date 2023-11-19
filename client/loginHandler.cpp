#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::logOutUser(){
    Packet packet;
    packet.type = P_LOGOUT_REQUEST;
    packet.size = 0;
    sendData(*socket, packet);
    delete(packet);
}

void MainWindow::checkLoginUser(){
    Packet packet;
    QString login = ui->loginLineEdit->text();
    packet.type = P_ASK_LOGIN_USER;
    packet.size = login.length();
    packet.data = const_cast<char*>(login.toStdString().c_str());
    sendData(*socket, packet);
}

void MainWindow::sendLogin(QString login){
    Packet packet;
    packet.type = P_SEND_LOGIN;
    packet.size = login.length();
    packet.data = const_cast<char*>(login.toStdString().c_str());
    sendData(*socket, packet);
}

void MainWindow::sendPassword(QString password){
    Packet packet;
    packet.type = P_SEND_PASSWORD;
    packet.size = password.length();
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
    packet.size = login.length();
    packet.data = const_cast<char*>(login.toStdString().c_str());
    sendData(*socket, packet);
}

void MainWindow::sendPasswordToLogin(QString password){
    Packet packet;
    packet.type = P_SEND_PASSWORD_LOG;
    packet.size = password.length();
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
}
