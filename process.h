#ifndef PROCESS_H_
#define PROCESS_H_

#include "httphead.h"

class Process {
    private:
        ;
    public:
        Processed() = default;
        Method GETprocess(CLIENT *cli);
        Method POSTprocess(std::string readbuf, Clientinfo *cli, std::string *info, std::string *location);
        POSTState Choess(std::string str_state) = 0;    
        std::string Serverstate(int state);
        virtual ~Processed() = default;
};

//GET只用于发送html/css/js...文件
Method Process::GETprocess(CLIENT *cli) {
    std::string filedir = DIR;//先添加文件的绝对位置
    std::string filename;
    std::cout << "Processing...";
    int beg = 5, end = 0;
    while (end <= 100) { //开始读取要求的文件位置
        if (httphead[end + beg] == ' ')
            break;
        end++;
    }
    if(end == 0) { //默认返回index.html
        filename = "index.html";
    }
    else if(end <= 100) { //截取文件名
        filename = httphead.substr(beg, end);
    }
    else { //文件地址要求过长
        std::cout << "Read fail.";
        return = ERROR;
    }
    std::cout << "Read... over. Method: GET "
              << "File: " << filename << " ";

    if(Readfile(filedir, cli)) { //读取成功
        Responehead(200, filename, cli);
        return 1;  
    }
    else { //open/stat出错
        return -1; 
    }
}
//写入http头之后所带的信息 和 前端POST请求的位置
int Process::POSTprocess(std::string readbuf, Clientinfo *cli, std::string *info, std::string *location) {
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
        return -1;
    }
    else {
        return 1;
    }

}

class Login :public Processed {
    public:
        Login() = default;
        virtual POSTState Choess(std::string str_state);
        ~Login() = default;
};
class Resetpassword :public Processed {

};
class Createaccount :public Processed {

};
class Vote :public Processed {
    
};
class Comment :public Processed {

};
class Content :public Processed {

};
class Readcount :public Processed {

}

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