#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    loginExists = false;
    password = false;

    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::connection);
    connect(ui->logInButton, &QPushButton::clicked, this, &MainWindow::loginUser);
    connect(ui->signUpButton, &QPushButton::clicked, this, &MainWindow::signUpUser);
}

MainWindow::~MainWindow()
{
    delete ui;
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
    QString login = ui->loginLineEdit->text();
    QString password = ui->passwordLineEdit->text();
    checkLoginUser();
    if(login.length()!=0 && password.length()!=0 && !loginExists){
        sendLogin(login);
        sendPassword(password);
    }else if (loginExists){
        ui->infoBox->setText("User already exists.");
    }
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
    QString login = ui->loginLineEdit->text();
    QString password = ui->passwordLineEdit->text();
    checkLoginUser();
    if(login.length()!=0 && password.length()!=0 && loginExists){
        sendLoginToLogin(login);
        sendPasswordToLogin(password);
    }else{
        ui->infoBox->setText("User doesn't exist.");
    }
}

void MainWindow::changeLoginSection(bool val){
    ui->loginLineEdit->setEnabled(val);
    ui->passwordLineEdit->setEnabled(val);
    ui->logInButton->setEnabled(val);
    ui->signUpButton->setEnabled(val);
    ui->logoutButton->setEnabled(!val);
}

void MainWindow::receivePacket(){
    QByteArray receivedData;
    Packet packet;
    while(socket->waitForReadyRead(50) || socket->bytesAvailable()>0){
        receivedData.append(socket->readAll());
        while(readData(receivedData, packet)){
            if(packet.type == P_LOGIN_USER){
                ui->infoBox->setText("Connection established. LogIn required.");
            }
            if(packet.type == P_USER_NOT_EXIST){
                loginExists = false;
            }
            if(packet.type == P_USER_EXIST){
                loginExists = true;
            }
            if(packet.type == P_SIGNUP_SUCCES){
                loginExists = true;
                password = true;
                ui->infoBox->setText("Signed up and logged in succesfully.");
                changeLoginSection(false);
            }
            if(packet.type == P_USER_ALREADY_LOGGED_IN){
                loginExists = false;
                password = false;
                ui->infoBox->setText("User already logged in.");
            }
            if(packet.type == P_LOGIN_SUCCES){
                loginExists = true;
                password = true;
                ui->infoBox->setText("Logged in succesfully.");
                changeLoginSection(false);
            }
            if(packet.type == P_WRONG_PASSWORD){
                loginExists = true;
                password = false;
                ui->infoBox->setText("Wrong password. Try again.");
            }
            receivedData.remove(0, 1024);
        }
    }
}

void MainWindow::connection()
{
    if(socket)
        delete socket;
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::connected, this, &MainWindow::socketConnected);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::receivePacket);
    socket->connectToHost(ui->adresLineEdit->text(), ui->portLineEdit->value());
}

void MainWindow::socketConnected(){
    ui->adresLineEdit->setEnabled(false);
    ui->portLineEdit->setEnabled(false);
    ui->connectButton->setEnabled(false);
    ui->loginLineEdit->setEnabled(true);
    ui->passwordLineEdit->setEnabled(true);
    ui->logInButton->setEnabled(true);
    ui->signUpButton->setEnabled(true);
    ui->infoBox->setText("Connection established");
}


