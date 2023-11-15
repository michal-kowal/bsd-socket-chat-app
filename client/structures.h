#ifndef STRUCTURES_H
#define STRUCTURES_H

enum class packetType{
    P_LOGIN_USER
};

struct Packet{
    packetType type;
    int size;
    char* data;
};

#endif