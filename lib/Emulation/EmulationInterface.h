// TODO move to package
#if not defined(EMULATION_INTERFACE_H)
#define EMULATION_INTERFACE_H

#include <vector>
#include <any>
#include <map>
#include <MethodProfile.h>
#include <iostream>
#include <ostream>
#include <unistd.h>
using namespace std;

class EmulationInterface {
public:
    int _wait;

    int _lastPsuedoException;
    
    vector<MethodProfile> _methods;

    vector<std::map<const char *, uint16_t>> _exceptions;

    virtual void waits(int seconds) = 0;

    virtual void await() = 0;

    virtual void dumpMethods() = 0;

    // virtual void returns(const char * func, any var_t) = 0;
    
    virtual void setException(const char * func, uint16_t exception) = 0;

    virtual void reset() = 0;

    virtual void setInternalException(int exception) = 0;

    virtual int throwException(const char * func) = 0;

    // virtual void setMethod(const char * func, void (*function)(),  std::any var_t) = 0;

    virtual void invokeMethod(const char * methodName) = 0;

    template<typename T>
    T mock(const char * func);

    template<typename T>
    T doReturn(const char * func);
};

#endif
