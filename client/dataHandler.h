#ifndef DATAHANDLER_H
#define DATAHANDLER_H
#include <QTcpSocket>
#include <QByteArray>
#include "structures.h"

const int DATA_SIZE = 1024;
const int MAX_DATA_SIZE=DATA_SIZE-sizeof(int)-sizeof(int);

bool readData(const QByteArray &array, Packet &packet);

void deletePacket(Packet &packet);

bool sendData(QTcpSocket &socket, Packet &packet);

#endif // DATAHANDLER_H
