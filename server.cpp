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
    input.close();
}

void Server::sendMessage(int socket, std::string message) {
    send(socket, message.c_str(), message.length(), 0);
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
    char buffer[4028] = {0};
    int socketRead;

    socketRead = read(socket, buffer, 4028);
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
    userLock.lock();

    User user(userName, password);
    user.setConnectionSocket(socket);
    user.updateStatus(true);

    this->registeredUsers.push_back(user);
    userLock.unlock();
    
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
            userLock.lock();

            this->sendMessage(socket, "Success");

            this->registeredUsers.at(k).setConnectionSocket(socket);
            this->registeredUsers.at(k).updateStatus(true);

            std::cout << "Successful login attempt\n";
            userLock.unlock();
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
            userLock.lock();
            this->sendMessage(socket, "Success");
            std::cout << "Successfully logged out for user\n\n";

            this->registeredUsers.at(i).updateStatus(false);
            this->registeredUsers.at(i).setConnectionSocket(-1);
            userLock.unlock();
            return;
        }
    }

    this->sendMessage(socket, "Fail");
}

void Server::updateSubscription(int socket) {
    this->sendMessage(socket, "Ok");
    std::string message = this->receiveMessage(socket);
    std::string user;


    long unsigned int i =0; 
    while(message[i] != ' ')
        user += message[i++];
    i++; //Skip over the space

    std::string location = "";
    while(i < message.length())
        location += message[i++];
    

    for(size_t t =0; t < this->registeredUsers.size(); t++)
        if(this->registeredUsers.at(t).getUsername() == user) {
            userLock.lock();

            this->registeredUsers.at(t).addLocation(location);
            this->sendMessage(socket, "Success");
            std::cout << "Successfully subscribed to location\n";
            userLock.unlock();
            return;
        }

    this->sendMessage(socket, "Fail");
    std::cout << "Failed to subscribe to location\n";
    
}

void Server::removeSubscription(int socket) {
    this->sendMessage(socket, "Ok");

    std::string message = this->receiveMessage(socket);
    std::string user;


    long unsigned int i =0; 
    while(message[i] != ' ')
        user += message[i++];
    i++; //Skip over the space

    std::string location = "";
    while(i < message.length())
        location += message[i++];
    
    for(size_t t =0; t < this->registeredUsers.size(); t++)
        if(this->registeredUsers.at(t).getUsername() == user) {
            userLock.lock();

            this->registeredUsers.at(t).unsubscribeFromLocation(location);
            this->sendMessage(socket, "Success");
            std::cout << "Successfully unsubscribed from location\n";
            userLock.unlock();
            return;
        }

    this->sendMessage(socket, "Fail");
    std::cout << "Failed to unsubscribe from location\n";
}

void Server::changeUserPassword(int socket) {
    this->sendMessage(socket, "Ok");

    std::string message = receiveMessage(socket);
    std::string userName = "";
    std::string oldPassword = "";
    std::string newPassword = "";
    
    long unsigned int i =0; 
    while(message[i] != ' ')
        userName += message[i++];
    i++; //Skip over the space
    while(message[i] != ' ')
        oldPassword += message[i++];
    i++;
    while(i < message.length())
        newPassword += message[i++];
    
    for(size_t k =0; k < this->registeredUsers.size(); k++) {
        if(this->registeredUsers.at(k).getUsername() == userName && this->registeredUsers.at(k).getPassword() == oldPassword) {
            userLock.lock();

            this->sendMessage(socket, "Success");

            this->registeredUsers.at(k).setPassword(newPassword);
            std::cout << "Successful password change\n";

            userLock.unlock();
            return;
        }
    }

    this->sendMessage(socket, "Fail");
}

void Server::listUserSubscription(int socket) {
    this->sendMessage(socket, "Ok");
    std::string userName = this->receiveMessage(socket);

    std::string finalMessage = "";
    std::vector<std::string> subscribedLocations;

    for(size_t i =0; i < this->registeredUsers.size(); i++) {
        if(this->registeredUsers.at(i).getUsername() == userName)
            subscribedLocations = this->registeredUsers.at(i).getSubscribedLocations();
    }
    if(subscribedLocations.size() == 0)
        finalMessage = "None";
    else{
        for(size_t i =0; i < subscribedLocations.size(); i++) {
            if(i == subscribedLocations.size() - 1)
                finalMessage += subscribedLocations.at(i);
            else
                finalMessage += subscribedLocations.at(i) + ",";
        }
    }

    this->sendMessage(socket, finalMessage);
}

void Server::registerCommunicationSocket(int socket) {
    this->sendMessage(socket, "Ok");
    std::string userName = this->receiveMessage(socket);

    for(size_t i =0; i < this->registeredUsers.size(); i++)
        if(this->registeredUsers.at(i).getUsername() == userName){
            userLock.lock();

            this->registeredUsers.at(i).setCommunicationSocket(socket);
            userLock.unlock();
            this->sendMessage(socket, "Success");
            return;
        }
    
    this->sendMessage(socket, "Fail");
}

void Server::handleMessaging(int socket) {
    this->sendMessage(socket, "Ok");
    std::string sender = "";
    std::string receiver = "";
    std::string fullText = this->receiveMessage(socket);
    std::string incomingMessage = "";
    std::string outgoingMessage = "";

    std::cout << "Message: " << fullText << std::endl;

    long unsigned int i =0; 
    while(fullText[i] != ' ')
        sender += fullText[i++];
    i++;
    while(fullText[i] != ' ')
        receiver += fullText[i++];
    i++;
    while(i < fullText.length())
        incomingMessage += fullText[i++];
    
    outgoingMessage = "From: " + sender +" Message: " + incomingMessage;

    //Verify receiver is online
    for(size_t j =0; j < this->registeredUsers.size(); j++)
        if(this->registeredUsers.at(j).getUsername() == receiver) {
            if(!this->registeredUsers.at(j).isOnline()) {
                this->sendMessage(socket, "User is offline, message will not be sent");
                std::cout << "Offline User: Invalid request\n";
                return;
            } else {
                this->sendMessage(this->registeredUsers.at(j).getCommunicationSocket(), outgoingMessage);
                this->sendMessage(socket, "Message successfully delivered");
                return;
            }
        }
    
    this->sendMessage(socket, "Failed to send message, user does not exist!");
}

void Server::disconnectCommunicationSocket(int socket) {
    std::string user = this->receiveMessage(socket);

    for(size_t i =0; i < this->registeredUsers.size(); i++)
        if(this->registeredUsers.at(i).getUsername() == user)
            this->sendMessage(this->registeredUsers.at(i).getCommunicationSocket(), "Exit");
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
        else if(requestOperation == "password")
            this->changeUserPassword(socket);
        else if(requestOperation == "subscribe")
            this->updateSubscription(socket);
        else if(requestOperation == "unsubscribe")
            this->removeSubscription(socket);
        else if(requestOperation == "list")
            this->listUserSubscription(socket);
        else if(requestOperation == "Exit"){
            this->disconnectCommunicationSocket(socket);
            return;
        }
        else if(requestOperation == "message")
            this->handleMessaging(socket);
        else if(requestOperation == "listen") {
            this->registerCommunicationSocket(socket);
            return;
        }
        else {
            printf("Invalid request received\n");
            this->sendMessage(socket, "Invalid");
        }
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