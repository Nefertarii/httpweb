#include "serverprocess.h"

class Server Server_GWC(SERV_PORT);

void Stop(int sig) { //ªÒ»°–≈∫≈ 2
    std::cout << "\nInterrupt signal (" << sig << ") received.\n"
              << std::endl;
    if (sig == SIGINT) {
        Server_GWC.~Server();
        exit(sig);
    }
}

int main(int argc, const char **argv) {
    Server_GWC.Start();
}   