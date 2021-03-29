#include "serverprocess.h"
int main(int argc,const char **argv) {
    
    if (!argv[2]) {
        class Server Server_GWC(SERV_PORT);
        Server_GWC.Start();
    }
    else {
        class Server Server_GWC(std::atoi(argv[2]));
        Server_GWC.Start();
    }
    

}   