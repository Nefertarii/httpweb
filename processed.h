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
    //session sockfd�ڹر�ʱ����
    //�������ÿ��д��ɺ���
    //�޸ĺ�ǵ��޸�Server���е�Resetinfo()
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

//�Ե�¼���ݽ��н�ȡ���ж�
//�ɹ�����1 ���򷵻�-1
int Loginprocessed(std::string userinfo,std::string *username, std::string *password) {
    int beg = 9, end = 0;
    while (1) {
        if (userinfo[beg + end] == '&') {
            username->assign(userinfo, beg, end);
            break;
        }
        if (end == 51) {//���ȡ50λ����/����
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
        if (end == 21) {//���ȡ20λ����
            return -1;
        }
        end++;
    }
    return 1;
}
//������֤��





//GETֻ���ڷ����ļ�
int GET(Clientinfo *cli) {
    std::string filedir = DIR;//������ļ��ľ���λ��
    std::string filename = GETprocessed(readbuf);
    filedir += filename;
    std::cout << "Read over. Method: GET"
              << "   File: " << filename;
    int n = Readfile(filedir, cli); //��ȡ�����ļ�
    if(n == 1) { //��ȡ�ɹ�
        Responehead(200, filename, cli);
        return 1;  
    }
    else { //open/stat����
        return -1; 
    }
}

//д��httpͷ֮����������Ϣ �� ǰ��POST�����λ��
int POST(std::string readbuf, Clientinfo *cli, std::string *info, std::string *location) {
    //��ȡ����.
    int beg = 0, end = readbuf.length(); 
    while (beg < 200) { //��ȡ���������
        if (readbuf[end - beg] == '\n')
            break;
        beg++;
    }
    if (beg <= 200 && 0 < beg) { //��β����ʼ�н�ȡ����
        *info = readbuf.substr((end - beg + 1), beg);
    }
    else { //���ݹ�����û������
        *info = "ERROR";
    }

    //��ȡλ�� λ�ò�ͬ����ʽ��ͬ
    beg = 6, end = 0;
    while (end <= 100) { //��ȡ�����λ��
        if (readbuf[end + beg] == ' ')
            break;
        end++;
    }
    if (end <= 100 && 0 < end) { //��ͷ�н�ȡλ��
        *location = readbuf.substr(beg, end);
    }
    else { //Ҫ�������û������
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