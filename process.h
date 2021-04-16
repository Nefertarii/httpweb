#ifndef PROCESS_H_
#define PROCESS_H_

#include "httphead.h"


class Process {
    private:
        ;
    public:
        Processed() = default;
        int GET(Clientinfo *cli);
        int POST(std::string readbuf, Clientinfo *cli, std::string *info, std::string *location);
        POSTState Choess(std::string str_state) = 0;    
        virtual ~Processed() = default;

    protected:
        std::string Serverstate(int state);
};
std::string Process::Serverstate(int state) { //����httpͷ�е�״̬��ѡ��
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
//GETֻ���ڷ���html/css/js...�ļ�
int Process::GET(Clientinfo *cli) {
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
int Process::POST(std::string readbuf, Clientinfo *cli, std::string *info, std::string *location) {
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



void Infoprocess(std::string location) {
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
int Loginprocess(std::string userinfo,std::string *username, std::string *password) {
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
    else if(location == "voteup"){
        //...
        if (1) {

        }
        else {
            
        }
    }
    else if(location == "votedown") {

    }
    else if(location == "comment") {

    }
    else if(location == "contentmore") {

    }
    else if(location == "readcount") {

    }
    else {
        
    }
}
#endif