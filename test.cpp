#include <cstring>
#include <fstream>
#include <iostream>
#include <cassert>
#include <random>
#include <memory>

#define CLIENT std::shared_ptr<Clientinfo>

enum Method {
    Login_Fail = -5,
    Reset_Fail = -4,
    Create_Fail = -3,
    ERROR = -1,
    GET = 1,
    POST = 2,
    Login_OK = 3,
    Reset_OK = 4,
    Create_OK = 5
};

struct Clientinfo {
    std::string httphead;
    std::string info;
    int remaining;
    int send;
    int filefd;
    int sockfd;
    bool session;
    Clientinfo() : httphead("none"), info("none"), remaining(0), send(0), filefd(0), sockfd(0), session(false){}
    Clientinfo(const Clientinfo &tmp) : httphead(tmp.httphead), info(tmp.info), remaining(tmp.remaining), send(tmp.send), filefd(tmp.filefd), sockfd(tmp.sockfd), session(tmp.session){}
    Clientinfo &operator=(struct Clientinfo &&tmp_) {
        httphead = tmp_.httphead;
        info = tmp_.info;
        sockfd = tmp_.sockfd;
        remaining = tmp_.remaining;
        send = tmp_.send;
        filefd = tmp_.filefd;
        session = tmp_.session;
        return *this;
    }
    ~Clientinfo() = default;
    //session sockfd在关闭时处理
    //其余的在每次写完成后处理
    //修改后记得修改Server类中的Resetinfo()
};

class Process { //各种处理
    private:
        std::string readbuf;

    public:
        Process() = default;
        virtual Method Choess(std::string str_state) = 0;    
        virtual ~Process() = default;

    protected:
        int GET(Clientinfo *cli);
        int POST(std::string readbuf, Clientinfo *cli, std::string *info, std::string *location);
        std::string Serverstate(int state);
};
class Login :public Process {
    public:
        Login() = default;
        virtual Method Choess(std::string str_state);
        ~Login() = default;
};
class Resetpassword :public Process {

};
class Createaccount :public Process {

};
class Vote :public Process {
    
};
class Comment :public Process {

};
class Content :public Process {

};
class Readcount :public Process {

};

void *evptr = nullptr;

int Readfile(std::string filename, CLIENT *cli) {
    
    return 1;
}


void add(CLIENT *cli) {
    evptr = cli;
    //epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}



using namespace std;
int main() {
    //string GET = "GET / HTTP/1.1\r\nHost: 39.102.58.82\r\nConnection: keep-alive\r\nPragma: no-cache\r\nCache-Control: no-cache\r\nDNT: 1\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.82 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\nAccept-Encoding: gzip, deflate\r\nAccept-Language: zh-CN,zh;q=0.9";
    //string POST = "POST /login HTTP/1.1\r\nHost: 39.102.58.82\r\nConnection: keep-alive\r\nContent-Length: 33\r\nCache-Control: max-age=0\r\nOrigin: http://39.102.58.82\r\nUpgrade-Insecure-Requests: 1\r\nDNT: 1\r\nContent-Type: application/x-www-form-urlencoded\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.90 Safari/537.36 Edg/89.0.774.63\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\nReferer: http://39.102.58.82/\r\nAccept-Encoding: gzip, deflate\r\nAccept-Language: zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6\r\n\r\nusername=123&password=123&submit=";
    /*Clientinfo cli1;
    Clientinfo cli2;
    CLIENT cli5 = make_shared<Clientinfo>();
    cout << cli5.use_count() << endl;
    cli5->filefd = 15;
    add(&cli5);
    cout << cli5.use_count() << endl;
    std::shared_ptr<Clientinfo> *cli = static_cast<shared_ptr<Clientinfo>*>(evptr);
    cout << cli5.use_count() << endl;
    cout << cli->get()->filefd << endl;
    evptr = nullptr;
    cli5 = nullptr;
    cout << cli5.use_count() << endl;*/
    std::string filedir;//先添加文件的绝对位置
    std::string filename;
    CLIENT *cli;
    cout<<Readfile(filedir, cli);
}
//cli->get()