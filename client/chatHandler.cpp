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
    packet.size = sizeof(data.length());
    if (ret == QMessageBox::Yes) {
        packet.type = P_YES;
        sendData(*socket, packet);
        Chat* chat = new Chat;
        chat->receiver = data;
        activeChats.push_back(chat);
        displayActiveChats();
    } else if (ret == QMessageBox::No) {
        packet.type = P_NO;
        sendData(*socket, packet);
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
        Chat* chat = new Chat;
        chat->receiver = data;
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
        if(user->receiver != ui->loginLineEdit->text())
            listWidget->addItem(user->receiver);
    }

    layout->addWidget(listWidget);
}

void MainWindow::presentMessages(std::vector<Message> mess){
    ui->chatTextBrowser->clear();
    for(auto &message: mess){
        if(message.sender == ui->loginLineEdit->text()){
            ui->chatTextBrowser->setTextColor(QColor(64, 64, 64));
        }
        else{
            ui->chatTextBrowser->setTextColor(QColor(0, 0, 0));
        }
        ui->chatTextBrowser->insertPlainText("<"+message.sender+">: "+message.text + "\n");
    }
}

void MainWindow::displayMessages(QListWidgetItem *item){
    QString selectedUser = item->text();
    for(auto &chat: activeChats){
        if(chat->receiver==selectedUser){
            activeChat = chat;
            ui->ChatWith->setText(QString("Chatting with %1").arg(activeChat->receiver));
            presentMessages(activeChat->messages);
            break;
        }
    }
    ui->closeChatButton->setEnabled(true);
    ui->sendButton->setEnabled(true);
}

void MainWindow::sendMessage(){
    QString message = ui->messageTextEdit->toPlainText();
    if(message.length()>0){
        Message m;
        m.sender = ui->loginLineEdit->text();
        m.text = message;
        activeChat->messages.push_back(m);
        Packet packet;
        packet.type = P_MESSAGE_DEST;
        packet.size = sizeof(activeChat->receiver.length());
        packet.data = const_cast<char*>(activeChat->receiver.toStdString().c_str());
        sendData(*socket, packet);

        Packet packet1;
        packet1.type = P_MESSAGE_TEXT;
        QByteArray byteArray = message.toUtf8();
        packet1.size = byteArray.size();
        packet1.data = new char[packet1.size];
        memcpy(packet1.data, byteArray.constData(), packet1.size);
        sendData(*socket, packet1);
        delete[] packet1.data;
        ui->messageTextEdit->clear();
        presentMessages(activeChat->messages);
    }
}

void MainWindow::receiveMessage(const char *mess){
    std::stringstream stream(mess);
    Message message;
    std::string sender, text;
    stream >> sender;
    std::getline(stream, text);
    message.sender = QString::fromStdString(sender);
    message.text = QString::fromStdString(text);
    qDebug() << "Message from: " <<message.sender<< " text: " << message.text;
    ui->infoBox->setText(QString("<font color='darkred'>New message from %1</font>").arg(message.sender));
    if(activeChat && activeChat->receiver==message.sender){
        activeChat->messages.push_back(message);
        presentMessages(activeChat->messages);
    }
    else{
        for(auto &chat: activeChats){
            if(chat->receiver == message.sender){
                chat->messages.push_back(message);
            }
        }
    }
}

void MainWindow::performDelete(QString name, int check){
    if(activeChat && activeChat->receiver==name){
        activeChat=nullptr;
        ui->sendButton->setEnabled(false);
        ui->chatTextBrowser->clear();
    }
    for (int i = 0; i < activeChats.size(); i++) {
        if (activeChats[i]->receiver == name) {
            delete activeChats[i];
            activeChats.erase(activeChats.begin() + i);
            if(check == 1){
                Packet packet;
                packet.type = P_CLOSE_CHAT;
                packet.size = sizeof(name.length());
                packet.data = const_cast<char*>(name.toStdString().c_str());
                sendData(*socket, packet);
            }
            break;
        }
    }
}

void MainWindow::deleteChat(){
    QLayoutItem* layoutItem = ui->activeChats->itemAt(0);

    if (layoutItem) {
        QWidget* widget = layoutItem->widget();

        if (widget && widget->metaObject()->className() == QLatin1String("QListWidget")) {
            QListWidget* listWidget = qobject_cast<QListWidget*>(widget);

            if (listWidget->selectedItems().isEmpty()) {
                qDebug() << "No item selected.";
                return;
            }

            QListWidgetItem* selectedItem = listWidget->currentItem();
            QString selectedText = selectedItem->text();

            // Tutaj możesz wykorzystać 'selectedText' lub zrobić cokolwiek innego z zaznaczonym elementem
            qDebug() << "Selected item: " << selectedText;
            performDelete(selectedText, 1);

            // Wyczyść wybrany element z listy
            delete listWidget->takeItem(listWidget->row(selectedItem));
        }
    }
}
