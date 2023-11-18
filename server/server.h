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

#include "structures.h"

class Server{
public:
    Server(int p);
    ~Server();
    void runServer();
    void handleClient(int clientSocket);
    void createUsersTable();
    bool checkUserInDb(std::string login);
    void logInUser(int clientSocket);
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
};

#endif