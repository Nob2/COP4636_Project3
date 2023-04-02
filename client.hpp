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

class client {
    private:
        int clientSocket;
        std::string nameOfHost;
        struct sockaddr_in server_address;
        std::string fileContents;
        struct hostent* retrieveHostName(std::string hostName);
    public:
        void connectToHost(std::string nameOfHost);
        bool retrieveFile(std::string fileName);
        void printLastFile();
        void closeConnection();
};

#endif