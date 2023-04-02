#include "server.hpp"
#include <fstream>
#include <arpa/inet.h>
#include <fstream>
#include <sstream>

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
    printf("Exporting \n");
    std::ofstream output;
    output.open(this->userFile);

    if(output.fail()) {
        printf("Failed to save users!");
        exit(1);
    }

    for(size_t i=0; i < this->registeredUsers.size(); i++) {
        output << this->registeredUsers.at(i).getUsername() << ' ' << this->registeredUsers.at(i).getPassword() << ' ';

        for(size_t i=0; i < this->registeredUsers.at(i).getSubscribedLocations().size(); i++) {
            if(i != 0)
                output << ' ';
            output << this->registeredUsers.at(i).getSubscribedLocations().at(i);
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

        printf("%s %s\n", userName, password);

        std::string location;
        User user(userName, password);
        user.updateStatus(false);

        while(lineProcess >> location){
            user.addLocation(location);
            printf("%s\n", location);
        }
        
        this->registeredUsers.push_back(user);
    }

    printf("User import completed\n");
    printf("%d\n", this->registeredUsers.size());
    input.close();
}

void Server::initalizeServer()
{
    //initalizeListenSocket();
    //defineSocketAddress();
    importUsers();
    //beginListening();
    exportUsers();
}

void Server::handleIndividualRequest(int socket)
{
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
    /** for (size_t i = 0; i < this->clientThreads.size(); i++)
    {
        this->clientThreads.at(i).join();
    } **/
    shutdown(listeningSocket, SHUT_RDWR);
}