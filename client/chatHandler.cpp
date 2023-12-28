#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::newConnectionDisplay(QString data){
    QMessageBox msgBox;
    msgBox.setWindowTitle("Chat request");
    msgBox.setText(QString("Do you accept a new chat request from %1").arg(data));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    int ret = msgBox.exec();
    Packet packet;
    packet.data = const_cast<char*>(data.toStdString().c_str());
    packet.size = data.length();
    if (ret == QMessageBox::Yes) {
        packet.type = P_YES;
        sendData(*socket, packet);
        Chat chat;
        chat.receiver = data;
        activeChats.push_back(chat);
        displayActiveChats();
        qDebug() << "Wciśnięto Yes";
    } else if (ret == QMessageBox::No) {
        packet.type = P_NO;
        sendData(*socket, packet);
        qDebug() << "Wciśnięto No";
    }
}

void MainWindow::ack(QString data, enum packetType type){
    QMessageBox msgBox;
    msgBox.setWindowTitle("Information");
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    if(type == P_NO){
        msgBox.setText(QString("User %1 did not accept your request").arg(data));
    }
    else if(type == P_YES){
        msgBox.setText(QString("User %1 accepted your request").arg(data));
        Chat chat;
        chat.receiver = data;
        activeChats.push_back(chat);
        displayActiveChats();
    }
    msgBox.exec();
}

void MainWindow::displayActiveChats(){
    QHBoxLayout* layout = ui->activeChats;
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    QListWidget* listWidget = new QListWidget();

    // Połączenie sygnału itemDoubleClicked z odpowiednim slotem
    connect(listWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::displayMessages);

    for(const auto &user: activeChats){
        if(user.receiver != ui->loginLineEdit->text())
            listWidget->addItem(user.receiver);
    }

    layout->addWidget(listWidget);
}

void MainWindow::presentMessages(std::vector<Message> mess){
    for(const auto &message: mess){
        qDebug() << message.text;
    }
}

void MainWindow::displayMessages(QListWidgetItem *item){
    QString selectedUser = item->text();
    for(const auto &chat: activeChats){
        if(chat.receiver==selectedUser){
            activeChat = chat;
            presentMessages(chat.messages);
            break;
        }
    }
    ui->sendButton->setEnabled(true);
}

void MainWindow::sendMessage(){
    QString message = ui->messageTextEdit->toPlainText();
    Packet packet;
    qDebug() << "send";
    packet.type = P_MESSAGE_DEST;
    packet.size = activeChat.receiver.length();
    packet.data = const_cast<char*>(activeChat.receiver.toStdString().c_str());
    sendData(*socket, packet);

    packet.type = P_MESSAGE_TEXT;
    packet.size = message.length();
    packet.data = const_cast<char*>(message.toStdString().c_str());
    sendData(*socket, packet);
}
