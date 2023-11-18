#include "server.h"
#include "structures.h"

bool Server::sendPacket(int socket, Packet &packet){
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

bool Server::receivePacket(int socket, Packet &packet){
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
    if (readBytes <= 0) {
            // Error or connection closed by client
            if (readBytes == 0) {
                std::cout << "Connection closed by client." << std::endl;
            } else {
                std::cerr << "Error reading from client." << std::endl;
            }
            return false;
        }
    return false;
}

void Server::deletePacket(Packet &packet){
    if(packet.size>0)
        delete[] packet.data;
}