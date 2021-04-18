#ifndef JSONPROCESS_H_
#define JSONPROCESS_H_


#include "httphead.h"

//根据状态码不同写入不同的json 
void Jsonprocess(int state,CLIENT *cli) {
    //判断 find name 成功 添加json
    if (state) {
        cli->get()->info =  "{\"Name\":\"gwc\",\"Age\":\"20\",\"session\":\"success\"}";
    }
    //失败
    else {
        cli->get()->info =  "{\"session\":\"fail\"}";
    }
    cli->get()->remaining += cli->get()->info.length();
}


#endif