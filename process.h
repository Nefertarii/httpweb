#ifndef PROCESS_H_
#define PROCESS_H_


#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <cstring>
#include <csignal>
#include <ctime>
#include <random>
#include <vector>
#include <memory>
#include "localinfo.h"
#include "jsonprocess.h"
#include "serverprocess.h"
#include "record.h"
#include "servhead.h"

extern const std::string DIR;
extern const std::string PAGE400;
extern const std::string PAGE401;
extern const std::string PAGE403;
extern const std::string PAGE404;

class Process {
    private:
        ;
    public:
        Process() = default;
        virtual Method Choess(std::string str_state) = 0;    
        std::string Serverstate(int state);
        virtual ~Process() = default;
};

//GET只用于发送html/css/js...文件
Method GETprocess(CLIENT *cli) {
    std::string filedir = DIR;//先添加文件的绝对位置
    std::string filename;
    std::cout << "Processing method:GET...";
    int beg = 5, end = 0;
    while (end <= 100) { //开始读取要求的文件位置
        if (cli->get()->readbuf[end + beg] == ' ')
            break;
        end++;
    }
    if(end == 0) { //默认返回index.html
        filename = "index.html";
    }
    else if(end <= 100) { //截取文件名
        filename = cli->get()->readbuf.substr(beg, end);
    }
    else if (end > 100)
    { //文件地址要求过长
        std::cout << " to long.";
        Readfile(PAGE404, cli);
        Responehead(200, PAGE404, cli);
        return OK;
    }
    std::cout << "done. "
              << "File: " << filename << " ";
    filedir += filename;
    if(Readfile(filedir, cli)) { //读取成功
        Responehead(200, filedir, cli);
    }
    else { //open/stat出错
        return ERROR;
    }
    return OK;
}
//写入http头之后所带的信息 和 前端POST请求的位置
Method POSTprocess(std::string readbuf, CLIENT *cli, std::string *info, std::string *location) {
    //获取数据.
    int beg = 0, end = readbuf.length(); 
    while (beg < 200) { //读取请求的数据
        if (readbuf[end - beg] == '\n')
            break;
        beg++;
    }
    if (beg <= 200 && 0 < beg) { //从尾部开始中截取数据
        *info = readbuf.substr((end - beg + 1), beg);
    }
    else { //数据过长或没有设置
        *info = "ERROR";
    }

    //获取位置 位置不同处理方式不同
    beg = 6, end = 0;
    while (end <= 100) { //读取请求的位置
        if (readbuf[end + beg] == ' ')
            break;
        end++;
    }
    if (end <= 100 && 0 < end) { //从头中截取位置
        *location = readbuf.substr(beg, end);
    }
    else { //要求过长或没有设置
        *location = "ERROR";
    }

    if (*location == "ERROR" || *info == "ERROR") {
        return ERROR;
    }
    else {
        return OK;
    }

}

class Login :public Process {
    public:
        Login(){}
        virtual Method Choess(std::string str_state);
        ~Login(){}
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
class Readcount :public Process{

};

//对登录数据进行截取、判断
//成功返回1 否则返回-1
int Loginprocess(std::string userinfo,std::string *username, std::string *password) {
    int beg = 9, end = 0;
    while (1) {
        if (userinfo[beg + end] == '&') {
            username->assign(userinfo, beg, end);
            break;
        }
        if (end == 51) {//最长读取50位名字/邮箱
            return -1;
        }
        end++;
    }
    beg = beg + end + 10;
    end = 0;
    while(1) {
        if (userinfo[beg + end] == '&') {
            password->assign(userinfo, beg, end);
            break;
        }
        if (end == 21) {//最长读取20位密码
            return -1;
        }
        end++;
    }
    return 1;
}
//返回验证码
#endif