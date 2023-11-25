#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::requestUsersList(){
    Packet packet;
    packet.type = P_REQUEST_USERS_LIST;
    packet.size = 0;
    sendData(*socket, packet);
}

void MainWindow::displayUsersList(){
    QListWidget* listWidget = new QListWidget();
    for(const auto &user: users){
        if(user != ui->loginLineEdit->text())
            listWidget->addItem(user);
    }
    QHBoxLayout* layout = ui->usersList;
    layout->addWidget(listWidget);
}
