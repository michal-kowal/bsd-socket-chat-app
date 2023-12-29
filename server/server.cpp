#include "server.h"
#include "structures.h"

const int one = 1;


Server::Server(int p){
    port = p;
    int exit = 0;
    exit = sqlite3_open("../chat.db", &DB);

    if(exit){
        std::cerr<<"Error open DB"<<sqlite3_errmsg(DB)<<std::endl;
        throw std::runtime_error("Finished");
    }
    else{
        std::cout<<"Opened database successfully!"<<std::endl;
        createUsersTable();
    }
}

Server::~Server(){
    std::cout<<"zakonczono prace"<<std::endl;
    sqlite3_close(DB);
}

void Server::runServer(){
    sockaddr_in localAdress{
        .sin_family = AF_INET,
        .sin_port = htons(1100),
        .sin_addr = {htonl(INADDR_ANY)}
    };

    int servSock = socket(PF_INET, SOCK_STREAM, 0);
    setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

    if (bind(servSock, (sockaddr*) &localAdress, sizeof(localAdress)))
        error(1, errno, "Could not bind!");

    listen(servSock, 5); // Adjust the backlog to the desired value

    // std::vector<std::thread> clientThreads;

    while (true) {
        int ackSock = accept(servSock, nullptr, nullptr);
        if (ackSock == -1) {
            std::cerr << "Error accepting connection." << std::endl;
            continue;
        }

        std::cout << "Connection accepted from client." << std::endl;

        // Start a new thread to handle the client
        std::thread clientThread(&Server::handleClient, this, ackSock);
        clientThread.detach();  // Allow the thread to run independently

    }

    close(servSock);
}

void Server::logInUser(int clientSocket){
    sendPacketUni(clientSocket, P_LOGIN_USER);
}

void Server::sendPacketUni(int clientSocket, enum packetType type){
    Packet packet;
    packet.type=type;
    packet.size=0;
    if(!sendPacket(clientSocket, packet)) std::cout<<"error sending packet\n";
}

std::string Server::modifyClientsVector(int socket){
    std::string name;
    for(int i = 0; i < clients.size(); i++){
        if(clients[i].fd==socket){
            name = clients[i].username;
            clients.erase(clients.begin() + i);
            break;
        }
    }
    return name;
}

void Server::sendUsersList(int socket){
    std::lock_guard<std::mutex> lock(vectorMutex);
    for(const auto &user: clients){
        Packet packet;
        packet.type = P_USERS_LIST;
        packet.data = const_cast<char*>(user.username.c_str());
        packet.size = sizeof(user.username.length());
        if(!sendPacket(socket, packet)) std::cout<<"error sending packet\n";
    }
    sendPacketUni(socket, P_USERS_LIST_END);
}

std::string Server::findUserByFd(int fd){
    for(const auto &user: clients){
        if(user.fd==fd){
            return user.username;
        }
    }
    return "not found";
}

int Server::findUserByName(std::string name){
    for(const auto &user: clients){
        if(user.username==name){
            return user.fd;
        }
    }
    return 0;
}

void Server::createChat(int source, int dest){ //fd1 source; fd2 - destination
    Packet packet;
    std::string name = findUserByFd(source);
    packet.type = P_NEW_CHAT_REQUEST;
    packet.data = const_cast<char*>(name.c_str());
    packet.size = sizeof(name.length());
    if(!sendPacket(dest, packet)) std::cout<<"error sending packet\n";
}

void Server::sendAck(int source, int dest, enum packetType type){
    Packet packet;
    std::string name = findUserByFd(source);
    packet.type = type;
    packet.data = const_cast<char*>(name.c_str());
    packet.size = sizeof(name.length());
    if(!sendPacket(dest, packet)) std::cout<<"error sending packet\n";
}

void Server::sendMessage(Message mess){
    std::stringstream stream;
    stream << mess.sender << " " << mess.text;
    std::string result = stream.str();

    Packet packet;
    packet.type=P_MESSAGE_SEND;
    packet.size = result.length();
    packet.data = new char[packet.size + 1];
    memcpy(packet.data, result.c_str(), packet.size);
    packet.data[packet.size] = '\0';

    if(!sendPacket(mess.receiver, packet)) std::cout<<"error sending packet\n";
    delete[] packet.data;
}

void Server::closeChat(int dest, std::string toClose){
    Packet packet;
    packet.type=P_CLOSE_CHAT;
    packet.size=sizeof(toClose.length());
    packet.data = const_cast<char*>(toClose.c_str());
    if(!sendPacket(dest, packet)) std::cout<<"error sending packet\n";
}

void Server::handleClient(int clientSocket) {
    logInUser(clientSocket);
    
    Client newClient;
    Message mess;
    while (true) {
        Packet receivedPacket;
        bool received = receivePacket(clientSocket, receivedPacket);
        if(!received) break;

        if(receivedPacket.type==P_ASK_LOGIN_USER){
            std::string login = receivedPacket.data;
            if(!checkUserInDb(login)){
                std::cout<<"nie istnieje\n"<<std::endl;
                sendPacketUni(clientSocket, P_USER_NOT_EXIST);
            }else{
                sendPacketUni(clientSocket, P_USER_EXIST);
            }
            deletePacket(receivedPacket);
        }
        if(receivedPacket.type==P_SEND_LOGIN){
            std::string login = receivedPacket.data;
            newClient.username = login;
            deletePacket(receivedPacket);
        }
        if(receivedPacket.type==P_SEND_PASSWORD){
            std::string password = receivedPacket.data;
            newClient.fd = clientSocket;
            newClient.password = password;
            insertUserToDb(newClient.username, newClient.password);
            std::lock_guard<std::mutex> lock(vectorMutex);
            clients.push_back(newClient);
            sendPacketUni(clientSocket, P_SIGNUP_SUCCES);
            deletePacket(receivedPacket);
        }
        if(receivedPacket.type==P_SEND_LOGIN_LOG){
            std::string login = receivedPacket.data;
            newClient.fd = clientSocket;
            newClient.username = login;
            deletePacket(receivedPacket);
        }
        if(receivedPacket.type==P_SEND_PASSWORD_LOG){
            std::string password = receivedPacket.data;
            newClient.password = password;
            bool status = checkUserLoggedIn(newClient.username, 1);
            if(status){
                sendPacketUni(clientSocket, P_USER_ALREADY_LOGGED_IN);   
            }else{
                if(checkUserPassword(newClient.username, newClient.password)){
                    updateUserStatus(newClient.username, 1);
                    sendPacketUni(clientSocket, P_LOGIN_SUCCES);
                    std::lock_guard<std::mutex> lock(vectorMutex);
                    clients.push_back(newClient);
                }else{
                    sendPacketUni(clientSocket, P_WRONG_PASSWORD);
                }
            }
            deletePacket(receivedPacket);
        }
        if(receivedPacket.type==P_LOGOUT_REQUEST){
            std::string username = modifyClientsVector(clientSocket);
            updateUserStatus(username, 0);
            sendPacketUni(clientSocket, P_LOGOUT_CONFIRM);
        }
        if(receivedPacket.type==P_REQUEST_USERS_LIST){
            sendUsersList(clientSocket);
        }
        if(receivedPacket.type==P_USERS_NEW_CHAT){
            int dest = findUserByName(receivedPacket.data);
            createChat(clientSocket, dest);
        }
        if(receivedPacket.type==P_YES){
            int dest = findUserByName(receivedPacket.data);
            sendAck(clientSocket, dest, P_YES);
        }
        if(receivedPacket.type==P_NO){
            int dest = findUserByName(receivedPacket.data);
            sendAck(clientSocket, dest, P_NO);
        }
        if(receivedPacket.type==P_MESSAGE_DEST){
            mess.receiver = findUserByName(receivedPacket.data);
            mess.sender = findUserByFd(clientSocket);
        }
        if(receivedPacket.type==P_MESSAGE_TEXT){
            mess.text = receivedPacket.data;
            std::cout<<"Message from: "<<mess.sender<<" To: "<<mess.receiver<<" text: "<<mess.text<<std::endl;
            sendMessage(mess);
        }
        if(receivedPacket.type==P_CLOSE_CHAT){
            int dest = findUserByName(receivedPacket.data);
            std::string toClose = findUserByFd(clientSocket);
            closeChat(dest, toClose);
        }
    }
    close(clientSocket);
    pthread_exit(NULL);
}

int main(int argc, char ** argv) {
    Server server(atoi(argv[1]));
    server.runServer();
    return 0;
}
