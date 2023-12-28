#ifndef _SERVER_H
#define _SERVER_H

#include <cstring>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <cstdlib>
#include <cstdio>
#include <error.h>
#include <iostream>
#include <thread>
#include <vector>
#include <sqlite3.h>
#include <stdexcept>
#include <sstream>
#include <mutex>

#include "structures.h"

class Server{
public:
    Server(int p);
    ~Server();

    void runServer();
    void handleClient(int clientSocket);

    void createUsersTable();
    bool checkUserInDb(std::string login);
    bool checkUserLoggedIn(std::string login, int status);
    bool checkUserPassword(std::string login, std::string password);
    void updateUserStatus(std::string username, int newStatus);

    void logInUser(int clientSocket);
    void sendPacketUni(int clientSocket, enum packetType type);
    void insertUserToDb(std::string username, std::string password);

    std::string modifyClientsVector(int socket);

    void sendUsersList(int socket);

    std::string findUserByFd(int fd);
    int findUserByName(std::string name);

    void createChat(int source, int dest);

    void sendAck(int source, int dest, enum packetType type);

    bool sendPacket(int socket, Packet &packet);
    bool receivePacket(int socket, Packet &packet);
    void deletePacket(Packet &packet);
private:
    const int DATA_SIZE = 1024;
    const int MAX_DATA_SIZE=DATA_SIZE-sizeof(int)-sizeof(int);
    int port;
    const int MAX_EVENTS = 10;
    sqlite3* DB;
    std::vector<Client> clients;

    std::mutex mutex;
    std::mutex vectorMutex;
};

#endif