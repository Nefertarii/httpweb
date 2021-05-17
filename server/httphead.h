#ifndef HTTPHEAD_H_
#define HTTPHEAD_H_

#include <string>

enum STATECODE
{
    StatusOK = 200,           //200 ��ȡ�ɹ��ҺϷ�
    StatusForbidden = 403,    //403 ��ֹ����
    StatusBadRequest = 400,   //400 �Ƿ�����
    StatusUnauthorized = 401, //401 ��Ҫ��¼ ��¼ʧ��
    StatusNotFound = 404      //404 ���ʴ���
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
