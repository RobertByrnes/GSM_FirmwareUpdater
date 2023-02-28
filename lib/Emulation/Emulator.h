#pragma once

// TODO move to package
#if not defined(EMULATOR_H)
#define EMULATOR_H

#include <string>
#include <vector>
#include <map>
#include <any>
#include <EmulationInterface.h>
#include <Exceptions/EmulationException.h>
#include <iostream>
#include <ostream>
#include <unistd.h>

unsigned int microseconds = 1000000;

using namespace std;

#define PSUEDO_EXCEPTION_NO_RET_VAL         (10000)
#define PSUEDO_EXCEPTION_NO_EXCEPT          (20000)

class Emulator : public EmulationInterface {
public:    
    Emulator() {}
    ~Emulator() {}

    /**
     * @brief Sets the inactive period for this class mock instance.
     * 
     * @param seconds int
     */
    void waits(int seconds) { _wait = seconds; }

    /**
     * @brief Pauses action for specified seconds. Specified seconds
     * are set for this mock instance using the waits function. 
     * 
     */
    void await() {
        if (_wait <= 0) {
            return;
        } 
        usleep(_wait * microseconds);
    }

    /**
     * @brief Create a map of { func name, return value } and add this
     * to the vector of return values assigned to this mock instance.
     * 
     * @param func const char *
     * @param var_t std::any
     */
    void returns(const char * func, any var_t) {
        std::map<const char *, any> returnMap { { func, var_t } };
        _returnTypes.push_back(returnMap);
    }

    /**
     * @brief Creates an exception map of function name and and integer
     * representation of an excpetion. The map is placed into the 
     * exception vector for this mock instance. 
     * 
     * @param func const char *
     * @param exception int (currently only integer supported)
     */
    void setException(const char * func, uint16_t exception) {     
        std::map<const char *, uint16_t> exceptionMap { { func, exception } };
        _exceptions.push_back(exceptionMap);
    }

    /**
     * @brief Resets the inactive period for the mock instance and clears
     * both _returnType and _exceptions vectors.
     * 
     */
    void reset() {
        _wait = 0;
        _returnTypes.clear();
        _exceptions.clear();
    }

    /**
     * @brief Record the last exception throw by the class
     * 
     * @param exception 
     */
    void setInternalException(int exception) { _lastPsuedoException = exception; }

    /**
     * @brief After setting the invokable method object
     * it is stored in the _methods vector of this
     * instance.
     * 
     * @param func const char *
     * @param function pointer
     */
    void setMethod(const char * methodName, void (*method)()) {
        Invokable invokableMethod = { methodName, method, 0 };
        _methods.push_back(invokableMethod);
    }

    /**
     * @brief Currently increments the call count.
     * 
     * @param methodName const char *
     */
    void invokeMethod(const char * methodName) {
        for(auto method : _methods) {
            if (method.methodName == methodName) {    
                method.invoked += 1;
                // method.method();
                break;
            }
        }
    }

    /**
     * @brief Run the mock method by calling the underlying functions.
     * Currently supporting waits, exceptions and return values.
     * 
     * @tparam T typename
     * @param func const char *
     * @return T typename
     * @throws exceptions int
     */
    template<typename T>
    T mock(const char * func) {
        await();
        int exception = throwException(func);
        if (exception > -1) {
            throw exception;
        }
        return doReturn<T>(func);
    }

    /**
     * @brief Search the return values vector for the function name,
     * if found will return the return value of the return type.
     * 
     * @tparam T typename
     * @param func const char *
     * @return T typename
     * @throws PSUEDO_EXCEPTION_NO_RET_VAL (10000)
     */
    template<typename T>
    T doReturn(const char * func) { 
        T value;
        bool canReturn = false;
        for (auto returnType : _returnTypes) {
            if (auto part = returnType.find(func); part != returnType.end()) {    
                value = std::any_cast<T>(part->second);
                canReturn = true;
                break;
            }
        }
        if (!canReturn) {
            setInternalException(PSUEDO_EXCEPTION_NO_RET_VAL);
            throw PSUEDO_EXCEPTION_NO_RET_VAL;
        }
        return value;
    }

    /**
     * @brief Search for an exception in expections vector for this mock instance.
     * If the function name is found in the exception map the exception
     * integer is thrown.
     * 
     * @param func const char *
     */
    int throwException(const char * func) {
        for (auto exception : _exceptions) {
            if (auto part = exception.find(func); part != exception.end()) {
                return part->second;
            }
        }
        setInternalException(PSUEDO_EXCEPTION_NO_EXCEPT);
        return -1;
    }

    protected:
    int _wait = 0;

    int _lastPsuedoException = 0;

    /**
     * @brief A store of methods for the mock class.
     * Methods are stored as a vector of 
     * function pointers.
     * 
     */
    vector<Invokable> _methods;

    /**
     * @brief Throwable exceptions for this mock instance. Stored as a std::map
     * of function name (const char *) and exception (int).
     * Currently only integer supported.
     * 
     */
    vector<std::map<const char *, uint16_t>> _exceptions;

    /**
     * @brief Return types for this mock instance. Stored as a std::map
     * of function name (const char *) and return value (std::any).
     * 
     */
    vector<std::map<const char *, any>> _returnTypes;
};

#endif
