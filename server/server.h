#ifndef _SERVER_H
#define _SERVER_H


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

#include "structures.h"

class Server{
public:
    Server(int p);
    ~Server();
    void runServer();
    static void handleClient(int clientSocket);
    void createUsersTable();
private:
    int port;
    sqlite3* DB;
};

#endif