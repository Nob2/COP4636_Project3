/**
 * Author: Nicholas Brown-Duncan
 * Course: COP4635 Project 3
 * Date: 03/25/23
 * Purpose: Header file for class client
*/


#ifndef CLIENT_HPP
#define CLIENT_HPP
#define PORT_NUMBER 60001

#include "string"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

class Client {
    private:
        int clientSocket;
        std::string nameOfHost;
        struct sockaddr_in server_address;
        std::string fileContents;
        struct hostent* retrieveHostName(std::string hostName);

        void printHeader();
        void sendMessage(std::string message);
        std::string receiveMessage();

        std::string userName;
        std::string password;
        bool isLogined = false;

        void registerUser();
        void loginUser();
        void logout();
        void subscribeLocation();
        void removeLocation();
        void updatePassword();

        bool acknowledgeRequest();
        bool acknowledgeResult();
    public:
        void connectToHost(std::string nameOfHost);
        void closeConnection();
        void messageServer();
};

#endif