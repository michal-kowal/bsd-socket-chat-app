#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::connection);
    connect(ui->logInButton, &QPushButton::clicked, this, &MainWindow::loginUser);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loginUser(){
    Packet packet;
    QString login = ui->loginLineEdit->text();
    packet.type = P_ASK_LOGIN_USER;
    packet.size = login.length();
    packet.data = const_cast<char*>(login.toStdString().c_str());
    sendData(*socket, packet);
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


