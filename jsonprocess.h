#ifndef JSONPROCESS_H_
#define JSONPROCESS_H_

std::string Jsonprocess(int state) {
    //�ж� find name �ɹ� ���json
    if (state) {
        return "\"Name\":\"gwc\",\"Age\":\"20\",\"session\":\"success\"";
    }
    //ʧ��
    else {
        return "\"session\":\"fail\"";
    }
}


#endif