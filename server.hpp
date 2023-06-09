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

        std::string loginUser(int connectionSocket);
        void logoutUser(int connectionSocket, std::string userName);
        std::string registerUser(int connectionSocket);
        void changeUserPassword(int socket, std::string user);

        void updateSubscription(int socket, std::string user);
        void removeSubscription(int socket, std::string user);
        void listUserSubscription(int socket, std::string user);
        void registerCommunicationSocket(int socket);

        void handleMessaging(int socket, std::string sender);
        void handleGroupMessaging(int socket, std::string sender);

        void listOnlineUsers(int socket);
        void listPreviousMessages(int socket, std::string user);

        void disconnectCommunicationSocket(int socket, std::string user);
    public:
        void initalizeServer();
        void handleRequests();
        void closeServer();
};


#endif