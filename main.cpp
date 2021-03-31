#include "serverprocess.h"
int main(int argc,const char **argv) {
    
    class Server Server_GWC(SERV_PORT);
    Server_GWC.Start();
}   