#include "servprocess.h"
#include <chrono>

//constant AAABBB
//Class AaaBbb
//function Aaabbb
//variable aaabbb

Server Server_GWC;
std::chrono::_V2::system_clock::time_point startime = std::chrono::system_clock::now();

void Stop(int sig)
{ //get signal 2
    std::cout << "\nInterrupt signal (" << sig << ") received.\n"
              << std::endl;
    if (sig == SIGINT)
        Server_GWC.Stop();
    std::chrono::_V2::system_clock::time_point endtime = std::chrono::system_clock::now();
    std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(endtime - startime);
    double elapsed = double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
    std::cout << "Server run total Time:" << elapsed << "sec" << std::endl;
    exit(sig);
}
//g++ main.cpp -lmysqlcppconn8 -o server
int main(int argc, const char **argv)
{
    signal(SIGINT, Stop);
    Server_GWC.Start();
}