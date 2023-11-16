#ifndef STRUCTURES_H
#define STRUCTURES_H

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

enum packetType{
    P_LOGIN_USER,
    P_REGISTER_USER
};

struct Packet{
    packetType type;
    int size;
    char* data;
};

#endif