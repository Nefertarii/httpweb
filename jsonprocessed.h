#ifndef JSONPROCESSED_H_
#define JSONPROCESSED_H_

//����״̬�벻ͬд�벻ͬ��json 
void Jsonprocessed(int state,Clientinfo *cli) {
    //�ж� find name �ɹ� ���json
    if (state) {
        cli->bodyjson =  "{\"Name\":\"gwc\",\"Age\":\"20\",\"session\":\"success\"}";
    }
    //ʧ��
    else {
        cli->bodyjson =  "{\"session\":\"fail\"}";
    }
    cli->remaining += cli->bodyjson.length();
}


#endif