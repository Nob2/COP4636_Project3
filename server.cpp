#include "server.hpp"
#include <fstream>
#include <arpa/inet.h>
#include <fstream>
#include <sstream>
#include <iostream>

/**
 * Author: Nicholas Brown-Duncan
 * Course: COP4635 Project 3
 * Date: 03/25/23
 * Purpose: Definition file for class Server
 */

void Server::defineSocketAddress()
{
    this->socketAddress.sin_family = AF_INET;
    this->socketAddress.sin_port = htons(PORT_NUMBER);
    this->lengthOfAddress = sizeof(this->socketAddress);

    struct hostent *hostName = gethostbyname("localhost");

    if (hostName == nullptr)
    {
        printf("Unable to determine hostName, exitting program\n");
        exit(1);
    }

    this->socketAddress.sin_addr.s_addr = *((unsigned long *)hostName->h_addr);

    int bindNumber = bind(listeningSocket, (struct sockaddr *)&this->socketAddress, sizeof(this->socketAddress));

    if (bindNumber == -1)
    {
        printf("Error binding the socket, exitting server\n");
        exit(1);
    }

    printf("I will be utilizing the IP address of: %s\n", inet_ntoa(socketAddress.sin_addr));
}

void Server::initalizeListenSocket()
{
    this->listeningSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (listeningSocket == -1)
    {
        printf("Error creating initial socket, exitting program\n");
        exit(1);
    }
}

void Server::beginListening()
{
    int listenResponse = listen(listeningSocket, 5);
    if (listenResponse == -1)
    {
        printf("Error listening for requests, exitting\n");
        exit(1);
    }
}

void Server::exportUsers() {
    std::ofstream output;
    output.open(this->userFile);

    if(output.fail()) {
        printf("Failed to save users!");
        exit(1);
    }

    for(size_t i=0; i < this->registeredUsers.size(); i++) {
        output << this->registeredUsers.at(i).getUsername() << ' ' << this->registeredUsers.at(i).getPassword() << ' ';

        for(size_t j=0; j < this->registeredUsers.at(i).getSubscribedLocations().size(); j++) {
            if(j != 0)
                output << ' ';
            output << this->registeredUsers.at(i).getSubscribedLocations().at(j);
        } 
        output << std::endl;
    }

    output.close();
}

void Server::importUsers() {
    std::ifstream input;
    input.open(this->userFile);

    if(input.fail()) {
        printf("Failed to load users, closing server\n");
        exit(1);
    }

    std::string line;

    while(std::getline(input, line)) {
        std::istringstream lineProcess(line);
        std::string userName;
        std::string password;
        std::vector<std::string> locations;

        lineProcess >> userName;
        lineProcess >> password;

        std::string location;
        User user(userName, password);
        user.updateStatus(false);

        while(lineProcess >> location){
            user.addLocation(location);
        }
        
        this->registeredUsers.push_back(user);
    }

    printf("User import completed\n");
    printf("%i\n", this->registeredUsers.size());
    input.close();
}

void Server::sendMessage(int socket, std::string message) {
    send(socket, message.c_str(), 1048, 0);
}

void Server::initalizeServer()
{
    initalizeListenSocket();
    defineSocketAddress();
    importUsers();
    beginListening();
    exportUsers();
}

std::string Server::receiveMessage(int socket) {
    char buffer[1024] = {0};
    int socketRead;

    socketRead = read(socket, buffer, 1024);
    if (socketRead == -1)
    {
        printf("Error communicating to socket %i, closing connection", socket);
        close(socket);
        return "";
    }

    return std::string(buffer);
}

void Server::registerUser(int socket) {
    //We got your request, send it along
    this->sendMessage(socket, "Ok");
    std::string message = receiveMessage(socket);
    std::string userName = "";
    std::string password = "";
    
    long unsigned int i =0; 
    while(message[i] != ' ')
        userName += message[i++];
    i++;
    while(i < message.length())
        password += message[i++];
    
    //Verify user doesn't already exist
    for(size_t k =0; k < this->registeredUsers.size(); k++) {
        if(this->registeredUsers.at(k).getUsername() == userName) {
            this->sendMessage(socket, "Fail");
            printf("User already exists, cannot register\n");
            return;
        }
    }

    std::cout << "Registration complete!\n\n";

    //User doesn't exist
    User user(userName, password);
    user.setConnectionSocket(socket);
    user.updateStatus(true);

    this->registeredUsers.push_back(user);
    this->sendMessage(socket, "Success");
}

void Server::loginUser(int socket) {
    this->sendMessage(socket, "Ok");
    std::string message = receiveMessage(socket);
    std::string userName = "";
    std::string password = "";
    
    long unsigned int i =0; 
    while(message[i] != ' ')
        userName += message[i++];
    i++; //Skip over the space
    while(i < message.length())
        password += message[i++];
    
    for(size_t k =0; k < this->registeredUsers.size(); k++) {
        if(this->registeredUsers.at(k).getUsername() == userName && this->registeredUsers.at(k).getPassword() == password) {
            this->sendMessage(socket, "Success");

            this->registeredUsers.at(k).setConnectionSocket(socket);
            this->registeredUsers.at(k).updateStatus(true);

            std::cout << "Successful login attempt\n";
            return;
        }
    }

    this->sendMessage(socket, "Fail");
}

void Server::logoutUser(int socket) {
    this->sendMessage(socket, "Ok");
    std::string userName = receiveMessage(socket);

    for(size_t i =0; i < this->registeredUsers.size(); i++) {
        if(this->registeredUsers.at(i).getUsername() == userName) {
            this->sendMessage(socket, "Success");
            std::cout << "Successfully logged out for user\n\n";

            this->registeredUsers.at(i).updateStatus(false);
            this->registeredUsers.at(i).setConnectionSocket(-1);
            return;
        }
    }

    this->sendMessage(socket, "Fail");
}

void Server::handleIndividualRequest(int socket)
{
    while(true){
        std::string requestOperation = receiveMessage(socket);
        std::cout << "Request: " << requestOperation << std::endl;

        if(requestOperation == "login")
            this->loginUser(socket);
        else if(requestOperation == "register")
            this->registerUser(socket);
        else if(requestOperation == "logout")
            this->logoutUser(socket);
        else if(requestOperation == "Exit")
            return;
        else
            printf("Invalid request received\n");
    }
}

void Server::handleRequests()
{
    while (true)
    {
        int communicationSocket = accept(listeningSocket, NULL, NULL);

        if (communicationSocket == -1)
        {
            printf("Error accepting connection\n");
            exit(1);
        }

        this->clientThreads.push_back(std::thread(&Server::handleIndividualRequest, this, communicationSocket));
    }
}

void Server::closeServer()
{
    exportUsers();
    for (size_t i = 0; i < this->clientThreads.size(); i++)
    {
        this->clientThreads.at(i).join();
    }
    shutdown(listeningSocket, SHUT_RDWR);
}