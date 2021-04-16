#ifndef JSONPROCESS_H_
#define JSONPROCESS_H_

//根据状态码不同写入不同的json 
void Jsonprocess(int state,Clientinfo *cli) {
    //判断 find name 成功 添加json
    if (state) {
        cli->bodyjson =  "{\"Name\":\"gwc\",\"Age\":\"20\",\"session\":\"success\"}";
    }
    //失败
    else {
        cli->bodyjson =  "{\"session\":\"fail\"}";
    }
    cli->remaining += cli->bodyjson.length();
}


#endif