/**
 * Author: Nicholas Brown-Duncan
 * Course: COP4635 Project 2
 * Date: 02/26/23
 * Purpose: Definition file for class clientManager
*/

#include "client.hpp"
#include <string.h>
#include <iostream>

struct hostent* Client::retrieveHostName(std::string nameOfHost) {
    this->nameOfHost = nameOfHost;
    struct hostent *hostName = gethostbyname(nameOfHost.c_str());

    if (hostName == nullptr)
    {
        printf("Unable to determine hostName, exitting program\n");
        exit(1);
    }

    server_address.sin_addr.s_addr = *((unsigned long *) hostName->h_addr);

    return hostName;
}

void Client::connectToHost(std::string hostName) {
    retrieveHostName(hostName);

    this->server_address.sin_family = AF_INET;
    this->server_address.sin_port = htons(PORT_NUMBER);

    if((clientSocket = socket(AF_INET, SOCK_STREAM,0)) < 0) {
        printf("Failed to create socket\n");
        exit(1);
    }

    int connectResult = connect(clientSocket, (struct sockaddr*) &server_address, sizeof(server_address));

    if(connectResult < 0) {
        printf("Failed to connect to the server\n");
        exit(1);
    }

    printf("Connection to server has succeed\n");
}

void Client::closeConnection() {
    close(clientSocket);
}

void Client::sendMessage( std::string message) {
    send(this->clientSocket, message.c_str(), 1048, 0);
}

std::string Client::receiveMessage() {
    char buffer[1024] = {0};
    int socketRead;

    socketRead = read(this->clientSocket, buffer, 1024);
    if (socketRead == -1)
    {
        printf("Error communicating to socket %d, closing connection", socket);
        close(socket);
        return "";
    }

    return std::string(buffer);
}

void Client::registerUser() {
    this->sendMessage("register");
    std::string acknowledgement = this->receiveMessage();

    if(acknowledgement != "Ok"){
        std::cout << "Error from server in acknowleding request\n" << std::endl;
        std::cout << "Received: " + acknowledgement << std::endl;
    }
}

void Client::messageServer() {
    while(true) {
        printHeader();
        int choice;
        std::cin >> choice;

        switch(choice) {
            case 1:
                this->registerUser();
                break;
            case 2: 
                break;
            case 3: 
                break;
            case 4: 
                break;
            case 5: 
                break;
            case 6:
                return;
                break;
            default:
                std::cout << "Invalid choice, try again\n";
                break;

        }
    }
}

void Client::printHeader() {
    std::cout << "What would you like to do?\n";
    std::cout << "1 - Register User \n";
    std::cout << "2 - Login User\n";
    std::cout << "3 - Log out\n";
    std::cout << "4 - Subscribe to new location\n";
    std::cout << "5 - Change password\n";
    std::cout << "6 - Exit Program\n";
    std::cout << std::endl;
}