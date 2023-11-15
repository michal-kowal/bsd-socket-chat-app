#include "server.h"

const int one = 1;

void Server::createUsersTable(){
    std::string sql = 
    "CREATE TABLE IF NOT EXISTS USER("
    "USERNAME TEXT PRIMARY KEY NOT NULL, "
    "STATUS INT NOT NULL)";
    char *messageError;
    if(sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError)!= SQLITE_OK){
        std::cerr<<"Error Create Table"<<std::endl;
        sqlite3_free(messageError);
    }
    else
        std::cout<<"Table created successfully"<<std::endl;
}

Server::Server(int p){
    port = p;
    int exit = 0;
    exit = sqlite3_open("chat.db", &DB);

    if(exit){
        std::cerr<<"Error open DB"<<sqlite3_errmsg(DB)<<std::endl;
        throw std::runtime_error("Finished");
    }
    else{
        std::cout<<"Opened database successfully!"<<std::endl;
        createUsersTable();
    }
}

Server::~Server(){
    std::cout<<"zakonczono prace"<<std::endl;
    sqlite3_close(DB);
}

void Server::runServer(){
    sockaddr_in localAdress{
        .sin_family = AF_INET,
        .sin_port = htons(1100),
        .sin_addr = {htonl(INADDR_ANY)}
    };

    int servSock = socket(PF_INET, SOCK_STREAM, 0);
    setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

    if (bind(servSock, (sockaddr*) &localAdress, sizeof(localAdress)))
        error(1, errno, "Could not bind!");

    listen(servSock, 5); // Adjust the backlog to the desired value

    // std::vector<std::thread> clientThreads;

    while (true) {
        int ackSock = accept(servSock, nullptr, nullptr);
        if (ackSock == -1) {
            std::cerr << "Error accepting connection." << std::endl;
            continue;
        }

        std::cout << "Connection accepted from client." << std::endl;

        // Start a new thread to handle the client
        std::thread clientThread(handleClient, ackSock);
        clientThread.detach();  // Allow the thread to run independently

    }

    close(servSock);
}

void Server::handleClient(int clientSocket) {
    const int bufferSize = 1024;
    char buffer[bufferSize];
    
    while (true) {
        int bytesRead = read(clientSocket, buffer, sizeof(buffer) - 1);
        if (bytesRead <= 0) {
            // Error or connection closed by client
            if (bytesRead == 0) {
                std::cout << "Connection closed by client." << std::endl;
            } else {
                std::cerr << "Error reading from client." << std::endl;
            }
            break;
        }

        buffer[bytesRead] = '\0';  // Null-terminate the received data
        std::cout << "Received from client: " << buffer << " fd: " << clientSocket << std::endl;
        // Echo the data back to the client
        write(clientSocket, buffer, bytesRead);
    }

    close(clientSocket);
}

int main(int argc, char ** argv) {
    Server server(atoi(argv[1]));
    server.runServer();
    return 0;
}
