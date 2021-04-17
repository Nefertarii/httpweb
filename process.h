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

//GETֻ���ڷ���html/css/js...�ļ�
Method Process::GETprocess(CLIENT *cli) {
    std::string filedir = DIR;//������ļ��ľ���λ��
    std::string filename;
    std::cout << "Processing...";
    int beg = 5, end = 0;
    while (end <= 100) { //��ʼ��ȡҪ����ļ�λ��
        if (httphead[end + beg] == ' ')
            break;
        end++;
    }
    if(end == 0) { //Ĭ�Ϸ���index.html
        filename = "index.html";
    }
    else if(end <= 100) { //��ȡ�ļ���
        filename = httphead.substr(beg, end);
    }
    else { //�ļ���ַҪ�����
        std::cout << "Read fail.";
        return = ERROR;
    }
    std::cout << "Read... over. Method: GET "
              << "File: " << filename << " ";

    if(Readfile(filedir, cli)) { //��ȡ�ɹ�
        Responehead(200, filename, cli);
        return 1;  
    }
    else { //open/stat����
        return -1; 
    }
}
//д��httpͷ֮����������Ϣ �� ǰ��POST�����λ��
int Process::POSTprocess(std::string readbuf, Clientinfo *cli, std::string *info, std::string *location) {
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
#endif