#ifndef JSONPROCESS_H_
#define JSONPROCESS_H_


#include "httphead.h"

//����״̬�벻ͬд�벻ͬ��json 
void Jsonprocess(int state,CLIENT *cli) {
    //�ж� find name �ɹ� ���json
    if (state) {
        cli->get()->info =  "{\"Name\":\"gwc\",\"Age\":\"20\",\"session\":\"success\"}";
    }
    //ʧ��
    else {
        cli->get()->info =  "{\"session\":\"fail\"}";
    }
    cli->get()->remaining += cli->get()->info.length();
}


#endif