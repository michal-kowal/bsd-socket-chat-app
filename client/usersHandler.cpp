#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::requestUsersList(){
    Packet packet;
    packet.type = P_REQUEST_USERS_LIST;
    packet.size = 0;
    sendData(*socket, packet);
}

void MainWindow::displayUsersList(){
    QHBoxLayout* layout = ui->usersList;
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    QListWidget* listWidget = new QListWidget();
    for(const auto &user: users){
        if(user != ui->loginLineEdit->text())
            listWidget->addItem(user);
    }

    layout->addWidget(listWidget);
}

void MainWindow::refreshUsersList(){
    users.clear();
    requestUsersList();
}
