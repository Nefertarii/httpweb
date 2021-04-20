#include "process.h"

//常量 AAABBB
//类 AaaBbb
//函数 Aaabbb
//变量 aaabbb

Server Server_GWC;

void Stop(int sig) { //获取信号 2
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