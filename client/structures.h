#ifndef STRUCTURES_H
#define STRUCTURES_H

enum packetType{
    P_LOGIN_USER,
    P_ASK_LOGIN_USER,
    P_USER_NOT_EXIST,
    P_USER_EXIST,
    P_SEND_LOGIN,
    P_SEND_PASSWORD,
    P_SEND_LOGIN_LOG,
    P_SEND_PASSWORD_LOG,
    P_SIGNUP_SUCCES,
    P_USER_ALREADY_LOGGED_IN,
    P_LOGIN_SUCCES,
    P_WRONG_PASSWORD
};

struct Packet{
    packetType type;
    int size;
    char* data;
};

#endif // STRUCTURES_H
