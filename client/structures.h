#ifndef STRUCTURES_H
#define STRUCTURES_H

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

#endif // STRUCTURES_H
