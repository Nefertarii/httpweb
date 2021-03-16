//#include "readfile.h"
//#include "httphead.h"
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <list>
#define MAX_BUF 1024

std::string file_porcess(std::string filename) {
    int index = filename.length(); 
    while(1) {
        if(filename[index]=='.')
            break;
        index--;
    }
    std::string suffix(filename, index, filename.length());
    if(suffix==".html")
        return "text/html";
    if(suffix==".data")
        return "application/json";
    if(suffix==".css")
        return "text/css";
    if(suffix==".js")
        return "text/javascript";
    if(suffix==".png")
        return "image/png";
    else
        return "text/plain";
}

using namespace std;
int main(int argc, const char **argv)
{
    std::string file = file_porcess("test.js");
    cout << file << endl;
}