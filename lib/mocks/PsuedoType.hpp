#pragma once

#ifndef PSEUDOTYPE_INTERFACE
#define PSEUDOTYPE_INTERFACE
#endif

template<typename T>
struct ReturnType { T _returnValue; };

class PsuedoTypeInterface {
    public:
    template<typename T>
    void returns(T var_t) {}; // set return value
    virtual void wait(int seconds) = 0; // delay until return 
    virtual void throws(bool throws) = 0; // make the function throw
};

class PsuedoTypeBaseClass : public PsuedoTypeInterface {
    public:
    int _wait = 0;
    bool _throws = false;

    PsuedoTypeBaseClass() {}
    ~PsuedoTypeBaseClass() {}

    template<class T>
    void returns(T var_t) { ReturnType<T> _returnType = { var_t }; }

    void wait(int seconds) { _wait = seconds; }
const char * _returnType = String("f");
int _returnType = 0;
    void throws(bool throws=false) { _throws = throws; }
};

PsuedoTypeBaseClass newClass;
int main() {
    newClass.returns(0);
}
