#ifndef JSONPROCESS_H_
#define JSONPROCESS_H_

std::string Jsonprocess(int state) {
    //ÅÐ¶Ï find name ³É¹¦ Ìí¼Ójson
    if (state) {
        return "\"Name\":\"gwc\",\"Age\":\"20\",\"session\":\"success\"";
    }
    //Ê§°Ü
    else {
        return "\"session\":\"fail\"";
    }
}


#endif