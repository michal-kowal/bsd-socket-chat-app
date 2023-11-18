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
    if(sendPacket(clientSocket, packet)) std::cout<<"packet sent\n";
    // deletePacket(packet);
}

void Server::sendUserNotExist(int clientSocket){
    Packet packet;
    packet.type=P_USER_NOT_EXIST;
    packet.size=0;
    if(!sendPacket(clientSocket, packet)) std::cout<<"error sending packet\n";
}

void Server::sendUserExist(int clientSocket){
    Packet packet;
    packet.type=P_USER_EXIST;
    packet.size=0;
    if(!sendPacket(clientSocket, packet)) std::cout<<"error sending P_USER_EXIST\n";
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
                sendUserNotExist(clientSocket);
            }else{
                sendUserExist(clientSocket);
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
            clients.push_back(newClient);
            deletePacket(receivedPacket);
        }
        if(receivedPacket.type==P_SEND_LOGIN_LOG){
            std::string login = receivedPacket.data;
            deletePacket(receivedPacket);
        }
        if(receivedPacket.type==P_SEND_PASSWORD_LOG){
            std::string password = receivedPacket.data;
            insertUserToDb(newClient.username, newClient.password);
            deletePacket(receivedPacket);
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
