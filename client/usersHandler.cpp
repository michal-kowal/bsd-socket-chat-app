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

    connect(listWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::onUserItemDoubleClicked);
    for(const auto &user: users){
        if(user != ui->loginLineEdit->text())
            listWidget->addItem(user);
    }

    layout->addWidget(listWidget);
}

void MainWindow::onUserItemDoubleClicked(QListWidgetItem *item) {
    bool check = true;
    QString selectedUser = item->text();
    for(auto &chat: activeChats){
        if(chat->receiver==selectedUser){
            check = false;
            break;
        }
    }
    if(check){
        Packet packet;
        packet.type = P_USERS_NEW_CHAT;
        packet.size = sizeof(selectedUser.length());
        packet.data = const_cast<char*>(selectedUser.toStdString().c_str());
        sendData(*socket, packet);
    }
}

void MainWindow::refreshUsersList(){
    users.clear();
    requestUsersList();
}
