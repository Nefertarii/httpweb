#ifndef PROCESSED_H_
#define PROCESSED_H_

#include "servhead.h"

#define DIR  "/home/ftp_dir/Webserver/Blog/"
#define PAGE400 "/home/ftp_dir/Webserver/Blog/Errorpage/Page400.html"
#define PAGE401 "/home/ftp_dir/Webserver/Blog/Errorpage/Page401.html"
#define PAGE403 "/home/ftp_dir/Webserver/Blog/Errorpage/Page403.html"
#define PAGE404 "/home/ftp_dir/Webserver/Blog/Errorpage/Page404.html"


struct Clientinfo {
    std::string httphead;
    std::string info;
    int remaining;
    int send;
    int filefd;
    int sockfd;
    bool session;
    Clientinfo operator=(struct Clientinfo tmp_) {
        struct Clientinfo tmp;
        tmp.httphead = tmp_.httphead;
        tmp.info = tmp_.info;
        tmp.sockfd = tmp_.sockfd;
        tmp.remaining = tmp_.remaining;
        tmp.send = tmp_.send;
        tmp.filefd = tmp_.filefd;
        tmp.session = tmp_.session;
        return tmp;
    }
    //session sockfd在关闭时处理
    //其余的在每次写完成后处理
    //修改后记得修改Server类中的Resetinfo()
};

std::string Headstate(int state) {
    switch (state)
    {
    case StatusOK:                  //200
        return " OK\r\n";
    case StatusBadRequest:          //400
        return " Bad Request\r\n";
    case StatusUnauthorized:        //401
        return " Unauthorized\r\n";
    case StatusForbidden:           //403
        return " Forbidden\r\n";
    case StatusNotFound:            //404
        return " Not Found\r\n";
    default:
        return " \r\n";
    }
}

void Infoprocessed(std::string location) {
    switch (state) {
    case StatusOK:                  //200
        return " OK\r\n";
    case StatusBadRequest:          //400
        return " Bad Request\r\n";
    case StatusUnauthorized:        //401
        return " Unauthorized\r\n";
    case StatusForbidden:           //403
        return " Forbidden\r\n";
    case StatusNotFound:            //404
        return " Not Found\r\n";
    default:
        return " \r\n";
    }
}

//对登录数据进行截取、判断
//成功返回1 否则返回-1
int Loginprocessed(std::string userinfo,std::string *username, std::string *password) {
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





//GET只用于发送文件
int GET(Clientinfo *cli) {
    std::string filedir = DIR;//先添加文件的绝对位置
    std::string filename = GETprocessed(readbuf);
    filedir += filename;
    std::cout << "Read over. Method: GET"
              << "   File: " << filename;
    int n = Readfile(filedir, cli); //读取本地文件
    if(n == 1) { //读取成功
        Responehead(200, filename, cli);
        return 1;  
    }
    else { //open/stat出错
        return -1; 
    }
}

//写入http头之后所带的信息 和 前端POST请求的位置
int POST(std::string readbuf, Clientinfo *cli, std::string *info, std::string *location) {
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

enum POSTState {
    Login_Fail = -1,
    Reset_Fail = -2,
    Create_Fail = -3,
    Login_Done = 1,
    Reset_Done,
    Create_Done,
};

POSTState POSTprocessed(std::string location, std::string info, Clientinfo *cli) {
    if(location == "login") {
        //...
        if (1) {
            return Login_Done;
        }
        else {
            return Login_Fail;
        }
    }
    else if(location == "resetpassword") {
        //...
        if (1) {
            return Reset_Done;
        }
        else {
            return Reset_Fail;
        }
    }
    else if(location == "createaccount") {
        //...
        if (1) {
            return Create_Done;
        }
        else {
            return Reset_Fail;
        }
    }
    else if(location == "zan"){
        //...
        if (1) {

        }
        else {
            
        }
    }
    else if(location == "cai") {

    }
    else if(location == "pinlun") {

    }
    else if(location == "quanwen") {

    }
    else if(location == "fangwencishu") {

    }
    else {
        
    }
}
#endif