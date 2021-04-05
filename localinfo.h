#ifndef LOCALINFO_H_
#define LOCALINFO_H_

#include <fstream>

//在文件中找到对应的用户名及密码
//用户名找到且密码正确返回1 否则返回-1
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