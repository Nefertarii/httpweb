#ifndef LOCALINFO_H_
#define LOCALINFO_H_

#include <fstream>

//���ļ����ҵ���Ӧ���û���������
//�û����ҵ���������ȷ����1 ���򷵻�-1
int Finduserinfo(std::string username, std::string password)
{
    std::ifstream userinfo("Info/userinfo.txt", std::ios::in);
    char tmp[50] = {0};
    if (!userinfo) {
        return -1;
    }
    while (!userinfo.eof()) {
        userinfo.getline(tmp, 50);
        if (username == tmp) {
            userinfo.getline(tmp, 50);
            if (password == tmp) {
                return 1;
            }
            else {
                return -1;
            }
        }
        userinfo.getline(tmp, 50);
    }
    return -1;
}



#endif