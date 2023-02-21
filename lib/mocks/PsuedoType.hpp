#pragma once

#ifndef PSEUDOTYPE_INTERFACE
#define PSEUDOTYPE_INTERFACE
#endif

#include<string>
#include <vector>
#include <array>
#include <map>

using namespace std;

template<class T>
std::vector<std::map<std::string, T>> vector;


class PsuedoTypeInterface {
    public:
    template<typename T>
    void returns(T var_t) {} // set return value

    virtual void wait(int seconds) = 0; // delay until return 
    
    virtual void throws(bool throws) = 0; // make the function throw
};

class PsuedoTypeBaseClass : public PsuedoTypeInterface {
    public:

    bool _throws = false;
    int _wait = 0;

    PsuedoTypeBaseClass() {}
    ~PsuedoTypeBaseClass() {}

    template<typename T>
    void returns(string func, T var_t) {
        std::map<std::string, T> willReturn;
        willReturn = this->ReturnType(func, var_t);
        vector.push_back(this->ReturnType(func, var_t));
    }

    void wait(int seconds) { _wait = seconds; }

    void throws(bool throws=false) { _throws = throws; }
};

PsuedoTypeBaseClass newClass;
int main() {
    newClass.returns("someFunc", 0);
}
