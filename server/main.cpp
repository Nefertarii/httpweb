#include "serverprocess.h"
#include <chrono>
//constant AAABBB
//class AaaBbb
//func Aaabbb
//variable aaabbb
Server server;
auto startime = std::chrono::system_clock::now();

void Stop(int sig)
{ //catch signal 2
    if (sig == SIGINT)
        server.Stop();
    std::cout << "\nInterrupt signal (" << sig << ") received.\n"
              << std::endl;
    auto endtime = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endtime - startime);
    double elapsed = double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
    std::cout << "Server run total Time:" << elapsed << "sec" << std::endl;
    exit(sig);
}

int main(int argc, const char **argv)
{
    signal(SIGINT, Stop);
    server.Start();
}
