#ifndef READFILE_H_
#define READFILE_H_

#define MAX_BUF_SIZE 1024

std::list<std::string> readfile(std::string name, std::string filetype, int &length) {
    std::list<std::string> recv;
    std::string filename, line;
    std::fstream file;

    filename =  name;
    std::cout << filename;

    if(filetype == "image/png") {
        file.open(filename, std::ios::in);
        if(file) {
            while(!file.eof()) {
                char tmp[MAX_BUF_SIZE];
                file.getline(tmp, MAX_BUF_SIZE);
                recv.push_back(tmp);
                length += strlen(tmp);
            }
        std::cout << "               success." << std::endl;
        }
        else std::cout << "...           Fail!" << std::endl;
        file.close();
        return recv;
    }
    else {
        file.open(filename, std::ios::in);
        if(file) {
            std::cout << "...";
            while(!file.eof()) {
                char tmp[MAX_BUF_SIZE];
                file.getline(tmp, MAX_BUF_SIZE);
                recv.push_back(tmp);
                length += strlen(tmp);
            }
        std::cout << "               success." << std::endl;
        }
        else std::cout << "...           Fail!" << std::endl;
        file.close();
        return recv;
    }
}

#endif
