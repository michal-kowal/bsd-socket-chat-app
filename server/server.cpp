#include "server.h"
#include "structures.h"

const int one = 1;


Server::Server(int p){
    port = p;
    int exit = 0;
    exit = sqlite3_open("chat.db", &DB);

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

bool Server::sendPacket(int socket, Packet &packet){
    char data[DATA_SIZE];
    memcpy(data, &packet.type, sizeof(packet.size));
    memcpy(data+sizeof(packet.type), &packet.size, sizeof(packet.size));
    if(packet.size>0)
        memcpy(data+sizeof(packet.type)+sizeof(packet.size), packet.data,MAX_DATA_SIZE);
    
    int bytesSent = write(socket, data, DATA_SIZE);
    if(bytesSent==-1)
        return false;
    return true;
}

bool Server::receivePacket(int socket, Packet &packet){
    char data[DATA_SIZE];
    int readBytes = read(socket, data, DATA_SIZE);
    if(readBytes>0){
        int controlBytes = readBytes;
        while(readBytes<DATA_SIZE && readBytes > 0){
            readBytes+= read(socket,data+controlBytes,DATA_SIZE-controlBytes);
            controlBytes = readBytes;
        }

        if(readBytes == DATA_SIZE){
            memcpy(&packet.type, data, sizeof(packet.type));
            memcpy(&packet.size, data + sizeof(packet.type), sizeof(packet.size));
            if(packet.size>0){
                packet.data = new char[MAX_DATA_SIZE];
                memcpy(packet.data, data+sizeof(packet.type)+sizeof(packet.size),packet.size);
            }
            return true;
        }
    }
    if (readBytes <= 0) {
            // Error or connection closed by client
            if (readBytes == 0) {
                std::cout << "Connection closed by client." << std::endl;
            } else {
                std::cerr << "Error reading from client." << std::endl;
            }
            return false;
        }
    return false;
}

void Server::deletePacket(Packet &packet){
    if(packet.size>0)
        delete[] packet.data;
}

void Server::logInUser(int clientSocket){
    std::cout<<"Logowanie\n";
    Packet packet, receivedPacket;
    std::string test = "testowa wiadomosc";
    packet.type=P_LOGIN_USER;
    packet.data=const_cast<char*>(test.c_str());
    packet.size = sizeof(test.length());
    if(sendPacket(clientSocket, packet)) std::cout<<"packet sent\n";
    // deletePacket(packet);
}

void Server::handleClient(int clientSocket) {
    logInUser(clientSocket);
    
    while (true) {
        Packet packet;
        bool received = receivePacket(clientSocket, packet);
        if(!received) break;

        if(packet.type==P_ASK_LOGIN_USER){
            std::string login = packet.data;
            if(!checkUserInDb(login)) std::cout<<"nie istnieje\n"<<std::endl;
            deletePacket(packet);
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
