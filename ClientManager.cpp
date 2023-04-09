#include "client.hpp"


int main() {
    Client client;
    client.connectToHost("localhost");
    client.messageServer();
}