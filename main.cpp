#include "process.h"

//���� AAABBB
//�� AaaBbb
//���� Aaabbb
//���� aaabbb

Server Server_GWC;

void Stop(int sig) { //��ȡ�ź� 2
    std::cout << "\nInterrupt signal (" << sig << ") received.\n"
              << std::endl;
    if (sig == SIGINT) {
        //Server_GWC.~Server();
        exit(sig);
    }
}

int main(int argc, const char **argv) {
    signal(SIGINT, Stop);
    Server_GWC.Start();
}   