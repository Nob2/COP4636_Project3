#include "server.hpp"
#include "signal.h"

Server server;

void signal_callback_handler(int signum) {
        server.closeServer();
        exit(1);
}


int main() {
    // On Ctrl-C close the server and exit
    // Commented out thread joining, as if an active connection is still open the thread will not join
    signal(SIGINT, signal_callback_handler);

    server.initalizeServer();
    server.handleRequests();
}