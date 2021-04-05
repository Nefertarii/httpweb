#ifndef JSONPROCESS_H_
#define JSONPROCESS_H_

//根据状态码不同写入不同的json 
std::string Jsonprocess(int state) {
    //判断 find name 成功 添加json
    if (state) {
        return "\"Name\":\"gwc\",\"Age\":\"20\",\"session\":\"success\"";
    }
    //失败
    else {
        return "\"session\":\"fail\"";
    }
}


#endif