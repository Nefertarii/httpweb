#ifndef HTTPHEAD_H_
#define HTTPHEAD_H_

#include <string>

enum STATECODE
{
    StatusOK = 200,           //200 读取成功且合法
    StatusForbidden = 403,    //403 禁止访问
    StatusBadRequest = 400,   //400 非法请求
    StatusUnauthorized = 401, //401 需要登录 登录失败
    StatusNotFound = 404      //404 访问错误
};

enum HTTP_TYPE
{
    ERROR = -1,
    TNONE,
    GET,
    POST,
};

HTTP_TYPE Httptype(std::string httphead);
std::string GMTime();
std::string Filetype(std::string filename);
std::string Serverstate(int state);
std::string Responehead(int state, std::string filename, int bodylength);

#endif
