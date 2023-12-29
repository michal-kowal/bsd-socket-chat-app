#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <iostream>

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
    P_WRONG_PASSWORD,
    P_LOGOUT_REQUEST,
    P_LOGOUT_CONFIRM,
    P_REQUEST_USERS_LIST,
    P_USERS_LIST,
    P_USERS_LIST_END,
    P_USERS_NEW_CHAT,
    P_NEW_CHAT_REQUEST,
    P_YES,
    P_NO,
    P_MESSAGE_DEST,
    P_MESSAGE_TEXT,
    P_MESSAGE_SEND,
    P_CLOSE_CHAT
};

struct Packet{
    packetType type;
    int size;
    char* data;
};

struct Message{
    std::string sender;
    int receiver;
    std::string text;
};

struct Client{
    int fd;
    std::string username;
    std::string password;
};

#endif