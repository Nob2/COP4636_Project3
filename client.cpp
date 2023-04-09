/**
 * Author: Nicholas Brown-Duncan
 * Course: COP4635 Project 2
 * Date: 02/26/23
 * Purpose: Definition file for class clientManager
*/

#include "client.hpp"
#include <string.h>
#include <iostream>
#include <unistd.h>

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
    send(this->clientSocket, message.c_str(), strlen(message.c_str()), 0);
}

std::string Client::receiveMessage() {
    char buffer[4028] = {0};
    int socketRead;

    socketRead = read(this->clientSocket, buffer, 4028);
    if (socketRead == -1)
    {
        printf("Error communicating to socket, closing connection");
        close(this->clientSocket);
        return "";
    }

    return std::string(buffer);
}

bool Client::acknowledgeRequest() {
    std::string acknowledgement = this->receiveMessage();

    if(acknowledgement != "Ok"){
        std::cout << "Error from server in acknowleding request\n" << std::endl;
        std::cout << "Received: " + acknowledgement << std::endl;
        return false;
    }

    return true;
}

bool Client::acknowledgeResult() {
    std::string acknowledgement = this->receiveMessage();

    if(acknowledgement == "Success") {
        return true;
    }
    else {
        return false;
    }
}

void Client::registerUser() {
    this->sendMessage("register");

    if(!acknowledgeRequest())
        return;

    std::string user;
    std::string pass;
    std::cout << "What is your username?\n";
    std::cin >> user;

    std::cout << "What is your password?\n";
    std::cin >> pass;

    std::string finalMessage = user + " " + pass;
    this->sendMessage(finalMessage);

    bool operationSucceeded = this->acknowledgeResult();

    if(operationSucceeded) {
        std::cout << "Successfully registered your user, you are now logined\n";
        this->userName = user;
        this->password = pass;
        this->isLogined = true;
    }
    else {
        std::cout << "Failed to register user. User most likely already exists.\n\n";
    }
}

void Client::loginUser() {
    this->sendMessage("login");

    if(!acknowledgeRequest())
        return;

    std::string user;
    std::string pass;
    std::cout << "What is your username?\n";
    std::cin >> user;

    std::cout << "What is your password?\n";
    std::cin >> pass;

    std::string finalMessage = user + " " + pass;
    this->sendMessage(finalMessage);

    bool operationSucceeded = this->acknowledgeResult();

    if(operationSucceeded) {
        std::cout << "Successfully logged in.\n";
        this->userName = user;
        this->password = pass;
        this->isLogined = true;
        
        return;
    } else {
        std::cout << "Error logging in for specified user/password combination\n";

        return;
    }

    std::cout << "There was an error communicating with the server, most likely a time out issue. Please try again\n";
}

void Client::logout() {
    this->sendMessage("logout");

    if(!acknowledgeRequest())
        return;

    this->sendMessage(this->userName);
    bool operationSucceeded = this->acknowledgeResult();

    if(operationSucceeded) {
        std::cout << "Successfully logged out\n";
        this->isLogined = false;
        this->userName = "";
        this->password = "";
    } else {
        std::cout << "Error logging out\n";
    }
}

void Client::subscribeLocation() {
    this->sendMessage("subscribe");
    if(!acknowledgeRequest())
        return;

    std::string finalMessage = this->userName;
    std::cout << "Enter location: ";
    std::string location;
    std::cin >> location;

    finalMessage += " " + location;

    this->sendMessage(finalMessage);

    bool operationSucceeded = this->acknowledgeResult();

    if(operationSucceeded) {
        std::cout << "Successfully subscribed to location\n";
    } else {
        std::cout << "Error occured during subscribing\n";
    }
}

void Client::removeLocation() {
    std::cout << "Please note, operation can be successfully even if you are not subscribed to the given location\n\n";
    this->sendMessage("unsubscribe");

    std::string acknowledgement;

    if(!acknowledgeRequest())
        return;
    
    std::string location;
    std::cout << "Enter location: ";
    std::cin >> location;

    std::string finalMessage = this->userName;
    finalMessage += " " + location;

    this->sendMessage(finalMessage);

    bool operationSucceeded = this->acknowledgeResult();

    if(operationSucceeded) {
        std::cout << "Successfully unsubscribed from location\n";
    } else {
        std::cout << "Failed to unsubscribe from location\n";
    }
}

void Client::updatePassword() {
    this->sendMessage("password");

    if(!acknowledgeRequest())
        return;
    
    std::string newPassword;
    std::cout << "Enter new password: " << newPassword;

    std::string finalMessage = this->userName;
    finalMessage += " " + this->password + " " + newPassword;

    this->sendMessage(finalMessage);

     bool operationSucceeded = this->acknowledgeResult();

    if(operationSucceeded) {
        std::cout << "Successfully updated password\n";
        this->password = newPassword;
    } else {
        std::cout << "Failed to update password\n";
    }
}

void Client::messageServer() {
    while(true) {
        printHeader();

        int choice;
        std::cin >> choice;

        if(!this->isLogined){
            switch(choice) {
                case 1:
                    this->registerUser();
                    break;
                case 2: 
                    this->loginUser();
                    break;
                case 3: 
                    this->sendMessage("Exit");
                    return;
                default:
                    std::cout << "Invalid choice, try again\n";
                    break;
            }
        } else {
            switch(choice) {
                case 1:
                    this->logout();
                    break;
                case 2:
                    this->subscribeLocation();
                    break;
                case 3:
                    this->updatePassword();
                    break;
                case 4:
                    this->removeLocation();
                    break;
                case 5:
                    this->sendMessage("Exit");
                    return;
                default:
                    std::cout << "Invalid choice, try again\n\n";
                    break;
            } 
        }
    }
}

void Client::printHeader() {
    std::cout << std::endl << std::endl;
    if(!this->isLogined){
        std::cout << "What would you like to do?\n";
        std::cout << "Please Note some features may require you to login\n";
        std::cout << "1 - Register User \n";
        std::cout << "2 - Login User\n";
        std::cout << "3 - Exit Program\n";
        std::cout << std::endl;
    } else {
        std::cout << "1 - Log out\n";
        std::cout << "2 - Subscribe to new location\n";
        std::cout << "3 - Change password\n";
        std::cout << "4 - Unsubscribe from location\n";
        std::cout << "5 - Exit Program\n";
        std::cout << std::endl;
    }
}