#include "serverprocess.h"
#include <chrono>
//���� AAABBB
//�� AaaBbb
//���� Aaabbb
//���� aaabbb
Server Server_GWC;
auto startime = std::chrono::system_clock::now();

void Stop(int sig)
{ //��ȡ�ź� 2
    std::cout << "\nInterrupt signal (" << sig << ") received.\n"
              << std::endl;
    if (sig == SIGINT)
        Server_GWC.Stop();
    auto endtime = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endtime - startime);
    double elapsed = double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
    std::cout << "Server run total Time:" << elapsed << "sec" << std::endl;
    exit(sig);
}

int main(int argc, const char **argv)
{
    signal(SIGINT, Stop);
    Server_GWC.Start();
}