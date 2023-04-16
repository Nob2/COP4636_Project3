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
#include <thread>

class Client {
    private:
        int clientSocket;
        int communicationSocket;
        
        std::string nameOfHost;
        struct sockaddr_in server_address;
        std::string fileContents;
        struct hostent* retrieveHostName(std::string hostName);
        std::thread listenThread;

        void printHeader();
        void sendMessage(int socket, std::string message);
        std::string receiveMessage();

        std::string userName;
        std::string password;
        bool isLogined = false;

        void registerUser();
        void loginUser();
        void logout();
        void updatePassword();

        void subscribeLocation();
        void removeLocation();
        void listSubscriptions();

        void listenForMessages();
        void printIncomingMessages();

        void messageUser();
        void messageGroup();

        bool acknowledgeRequest();
        bool acknowledgeResult();
    public:
        void connectToHost(std::string nameOfHost);
        void closeConnection();
        void messageServer();
};

#endif