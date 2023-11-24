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
    Packet packet, receivedPacket;
    std::string test = "testowa wiadomosc";
    packet.type=P_LOGIN_USER;
    packet.data=const_cast<char*>(test.c_str());
    packet.size = sizeof(test.length());
    if(!sendPacket(clientSocket, packet)) std::cout<<"error sending packet P_LOGIN_USER\n";
    // deletePacket(packet);
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

void Server::handleClient(int clientSocket) {
    logInUser(clientSocket);
    
    Client newClient;

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
    }
    close(clientSocket);
    pthread_exit(NULL);
}

int main(int argc, char ** argv) {
    Server server(atoi(argv[1]));
    server.runServer();
    return 0;
}
