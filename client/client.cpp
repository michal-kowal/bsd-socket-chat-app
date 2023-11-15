#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <error.h>
#include <iostream>
#include <arpa/inet.h>
#include <cstring>

int main(int argc, char ** argv) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(1100);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)))
        error(1, errno, "Failed to connect");

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
