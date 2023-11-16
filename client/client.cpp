#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <error.h>
#include <iostream>
#include <arpa/inet.h>
#include <cstring>

#include "structures.h"

const int DATA_SIZE = 1024;
const int MAX_DATA_SIZE=DATA_SIZE-sizeof(int)-sizeof(int);


bool sendPacket(int socket, Packet &packet){
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

bool receivePacket(int socket, Packet &packet){
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
    return false;
}

void deletePacket(Packet &packet){
    if(packet.size>0)
        delete[] packet.data;
}



void logIn(int socket){
    Packet receivedPacket;
    if(receivePacket(socket, receivedPacket)){
        std::cout<<"Received: "<<receivedPacket.data<<std::endl;
    }
}

int main(int argc, char ** argv) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(1100);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)))
        error(1, errno, "Failed to connect");

    logIn(sock);

    while (true) {
        char data[1024];
        std::cout << "Enter message to send (type 'exit' to quit): ";
        std::cin.getline(data, sizeof(data));

        if (std::strcmp(data, "exit") == 0) {
            // Break the loop and exit the program if the user enters 'exit'
            break;
        }

        std::cout << "Sending message: " << data << std::endl;
        write(sock, data, strlen(data));

        // Read and display the response from the server
        char buffer[1024];
        int bytesRead = read(sock, buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::cout << "Received from server: " << buffer << std::endl;
        } else {
            std::cerr << "Error reading from server." << std::endl;
            break;
        }
    }

    close(sock);
    return 0;
}
