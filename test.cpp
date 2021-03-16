//#include "readfile.h"
//#include "httphead.h"
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <list>
#define MAX_BUF 1024

std::list<std::string> readfile(std::string name, std::string filetype, int &filesize)
{
    std::list<std::string> recv;
    std::string filename, line;
    std::ifstream file;

    filename = name;
    std::cout << filename;

    if (filetype == "image/png")
    {
        if(file) {
            file.open(filename, std::ifstream::binary);
            file.seekg(0, file.end);
            filesize = file.tellg();
            file.seekg(0, file.beg);
            char *buf = new char[filesize];
            file.read(buf, filesize);
            if(file)
                std::cout << "\nis end success." << std::endl;
            else
                std::cout << "only read" << file.gcount() << std::endl;
            recv.push_back(buf);
            file.close();
            return recv;
        }
    }
    return recv;
}
using namespace std;
int main(int argc, const char **argv)
{

    int filesize = 0;
    std::list<std::string> file = readfile("test.png", "image/png", filesize);
    cout << " filesize:" << filesize << endl;
    for (auto i : file)
        cout << i << endl;
}