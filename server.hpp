#ifndef SERVER_HPP
#define SERVER_HPP

#define PORT_NUMBER 60001
#define HTTP_PORT 443

#include "user.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <thread>
#include <string.h>
#include <mutex>

/**
 * Author: Nicholas Brown-Duncan
 * Course: COP4635 Project 3
 * Date: 03/25/23
 * Purpose: Header file for class Server.
 * Server is responsible for creating the intiial main socket that allows clients to communicate with us,
 * alongside spinning up threads upon client connection. 
*/

class Server {
    private:
        int listeningSocket;
        int lengthOfAddress;
        struct sockaddr_in socketAddress;

        std::vector<std::thread> clientThreads;

        std::vector<User> registeredUsers;
        std::mutex userLock;
        std::string userFile = "users.txt";

        void defineSocketAddress();
        void initalizeListenSocket();
        void beginListening();
        void handleIndividualRequest(int socket);
        void exportUsers();
        void importUsers();

        std::string receiveMessage(int socket);
        void sendMessage(int socket, std::string message);

        void loginUser(int connectionSocket);
        void logoutUser(int connectionSocket);
        void registerUser(int connectionSocket);
        void changeUserPassword(std::string userName, std::string newPassword);
        void updateSubscription(std::string userName, std::string location);
        void getUserSubscription(std::string userName);
    public:
        void initalizeServer();
        void handleRequests();
        void closeServer();
};


#endif