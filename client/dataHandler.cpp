#include "structures.h"
#include "dataHandler.h"

bool readData(const QByteArray &array, Packet &packet){
    if(array.size()>=DATA_SIZE){
        packet.type = *((packetType*)(array.data()));
        packet.size = *((int*)array.data()+sizeof(packetType));
        packet.data = (char*)(array.data()+sizeof(packetType)+sizeof(int));
        return true;
    }
    return false;
}

bool sendData(QTcpSocket &socket, Packet &packet){
    char data[DATA_SIZE];
    memcpy(data, &packet.type, sizeof(packet.size));
    memcpy(data+sizeof(packet.type), &packet.size, sizeof(packet.size));
    if(packet.size>0){
        memcpy(data+sizeof(packet.type)+sizeof(packet.size), packet.data, MAX_DATA_SIZE);
    }
    socket.write(data, DATA_SIZE);
    if(socket.waitForBytesWritten(300)) return true;
    else return false;
}

void deletePacket(Packet &packet){
    if(packet.size>0){
        delete[] packet.data;
    }
}

#include <QTcpSocket>
