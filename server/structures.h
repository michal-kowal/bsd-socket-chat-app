#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <iostream>

enum packetType{
    P_LOGIN_USER,
    P_ASK_LOGIN_USER,
    P_REGISTER_USER
};

struct Packet{
    packetType type;
    int size;
    char* data;
};

struct Client{
    int fd;
    std::string username;
};

#endif