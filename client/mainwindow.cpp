#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->refreshButton->setVisible(false);

    loginExists = false;
    password = false;
    status = false;

    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::connection);
    connect(ui->logInButton, &QPushButton::clicked, this, &MainWindow::loginUser);
    connect(ui->signUpButton, &QPushButton::clicked, this, &MainWindow::signUpUser);
    connect(ui->logoutButton, &QPushButton::clicked, this, &MainWindow::logOutUser);
    connect(ui->refreshButton, &QPushButton::clicked, this, &MainWindow::refreshUsersList);
    connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::sendMessage);
    ui->UsersOnline->setVisible(false);
    ui->ActiveChatName->setVisible(false);
    ui->ChatWith->setVisible(false);
    ui->messageTextEdit->setVisible(false);
    ui->sendButton->setVisible(false);
    ui->closeChatButton->setVisible(false);
    ui->chatTextBrowser->setVisible(false);
}

MainWindow::~MainWindow()
{
    if(socket) socket->close();
    if(status) logOutUser();
    delete ui;
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
                chooseProcedure();
            }
            if(packet.type == P_USER_EXIST){
                loginExists = true;
                chooseProcedure();
            }
            if(packet.type == P_SIGNUP_SUCCES){
                status = true;
                loginExists = true;
                password = true;
                ui->infoBox->setText("Signed up and logged in succesfully.");
                changeLoginSection(false);
                requestUsersList();
            }
            if(packet.type == P_USER_ALREADY_LOGGED_IN){
                loginExists = false;
                password = false;
                ui->infoBox->setText("User already logged in.");
            }
            if(packet.type == P_LOGIN_SUCCES){
                status = true;
                loginExists = true;
                password = true;
                ui->infoBox->setText("Logged in succesfully.");
                changeLoginSection(false);
                requestUsersList();
            }
            if(packet.type == P_WRONG_PASSWORD){
                loginExists = true;
                password = false;
                ui->infoBox->setText("Wrong password. Try again.");
            }
            if(packet.type == P_LOGOUT_CONFIRM){
                status = false;
                loginExists = false;
                password = false;
                ui->infoBox->setText("Logged out.");
                users.clear();
                changeLoginSection(true);
            }
            if(packet.type == P_USERS_LIST){
                users.push_back(QString(packet.data));
            }
            if(packet.type == P_USERS_LIST_END){
                displayUsersList();
                displayActiveChats();
            }
            if(packet.type == P_NEW_CHAT_REQUEST){
                newConnectionDisplay(QString(packet.data));
            }
            if(packet.type == P_YES || packet.type == P_NO){
                ack(QString(packet.data), packet.type);
            }
            if(packet.type == P_MESSAGE_SEND){
                receiveMessage(packet.data);
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


