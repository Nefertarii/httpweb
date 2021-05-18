#ifndef PROCESS_H_
#define PROCESS_H_

#include "servhead.h"
#include "../database/database.h"

class ReadProcess
{
private:
    CLIENT *cli;

public:
    ReadProcess(CLIENT *cli_p) { cli = cli_p; }
    int Read();
    int GETprocess();
    int POSTprocess();
    int POSTLogin();
    int POSTReset();
    int POSTRegister();
    int POSTVote();
    int POSTComment();
    int POSTContent();
    int POSTReadcount();
    int POSTChoess(SERV_STATE method);
    int POSTChoess(std::string method);
    std::string Serverstate(int state);
    ~ReadProcess() {}
};
class WriteProcess
{
private:
    CLIENT *cli;

public:
    WriteProcess(CLIENT *cli_p) { cli = cli_p; }
    int Writehead();
    int Writefile();
    int Writeinfo();
    ~WriteProcess() {}
};
#endif