#pragma once

#ifndef PSEUDOTYPE_INTERFACE
#define PSEUDOTYPE_INTERFACE
#endif

#include <string>
#include <vector>
#include <map>
#include <any>

#ifndef ARDUINO
#include <iostream>
#include <ostream>
#include <unistd.h>
unsigned int microseconds = 1000000;
#endif

class PsuedoTypeInterface {
    public:
    bool _throws;
    int _wait;
    std::vector<std::map<const char *, std::any>> _vector;
    bool _booleanReturnType;
    std::string _stringReturnType;
    String _StringReturnType;
    int _intReturnType;
    float _floatReturnType;
    IPAddress _ipAddressReturnType;

    virtual void returns(const char * func, std::any var_t) = 0; // set return value

    virtual void findReturnType(const char * func) = 0;

    // virtual void waits(int seconds) = 0; // delay until return 

    virtual void await() = 0;
    
    // virtual void throwBeforeReturn(bool throws) = 0; // make the function throw

    // virtual void reset() = 0;
};

class PsuedoTypeBaseClass : public PsuedoTypeInterface {
    public:
    bool _throws = false;
    int _wait = 0;
    std::vector<std::map<const char *, std::any>> _vector;
    bool _booleanReturnType;
    std::string _stringReturnType;
    String _StringReturnType;
    int _intReturnType;
    float _floatReturnType;
    IPAddress _ipAddressReturnType;
    
    PsuedoTypeBaseClass() {}
    ~PsuedoTypeBaseClass() {}

    void returns(const char * func, std::any var_t) {
        std::map<const char *, std::any> returnMap { { func, var_t } };
        _vector.push_back(returnMap);
    }

    void findReturnType(const char * func) {
        int vecSize = _vector.size();
        for (auto x : _vector) {
            if (auto search = x.find(func); search != x.end()) {
                std::cout << "Found " << search->first << std::endl;
            } else {    
                std::cout << "Not found\n";
            }
        }
    }

    void await() { usleep(_wait * microseconds); }

    // void throwBeforeReturn() { _throws = true; }

    // void reset() {
    //     _throws = false;
    //     _wait = 0;
    // }
};
