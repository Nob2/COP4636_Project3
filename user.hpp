/**
 * Author: Nicholas Brown-Duncan
 * Course: COP4635 Project 3
 * Date: 03/25/23
 * Purpose: Header file for class User.
 * Responsible for storing information regarding connected users
*/

#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <vector>

class User {
    private:
        int connectedSocket = -1;
        int communicationSocket = -1;
        std::string userName;
        std::string password;
        std::vector<std::string> subscribedLocations;
        std::vector<std::string> receivedMessages;
        bool Online;
    public:

        User(std::string userName, std::string password);

        int getConnectedSocket() {return this->connectedSocket;}
        void setConnectionSocket(int socket) {this->connectedSocket = socket;}

        int getCommunicationSocket() {return this->communicationSocket;}
        void setCommunicationSocket(int socket) {this->communicationSocket = socket;}

        std::string getUsername() {return this->userName;}
        void setUsername(std::string userName) {this->userName = userName;}

        std::string getPassword() {return this->password;}
        void setPassword(std::string password) {this->password = password;}

        std::vector<std::string> getSubscribedLocations() {return this->subscribedLocations;}
        void addLocation(std::string location);
        void unsubscribeFromLocation(std::string location);

        std::vector<std::string> getReceivedMessages() {return this->receivedMessages;}
        void addReceiveMessage(std::string message);

        void updateStatus(bool onlineStatus) {this->Online = onlineStatus;}
        bool isOnline() {return this->Online;}

        bool isSubscribedTo(std::string location);

};




#endif