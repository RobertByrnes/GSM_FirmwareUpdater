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

#ifdef EMULATOR_LOG
#include <Logger.h>
FILE *fp = freopen("emulation.log", "w", stdout);
#define LOG_2_TXT(x)                               log_out<const char *>(x);
#else 
#define LOG_2_TXT(x)
#endif

class Emulator : public EmulationInterface {
public:    
    Emulator() {}
    ~Emulator() {
        #ifdef EMULATOR_LOG
        fclose(fp);
        #endif
    }

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
     * @brief Create a MethodProfile instance for the method descibed by the func parameter.
     * The method will return the value stored in the instance of RetVal. An assumption
     * is made that the mock invocation of this method will always return this value
     * when called unless the behaviour is further specified with chained 
     * method calls to times() and then().
     * 
     * @see MethodProfile
     * @see RetVal
     * @param func const char *
     * @param var_t std::any
     * @return Emulator& 
     */
    Emulator& returns(const char * func, any var_t) {
        _lastFunc = func;
        RetVal retVal = { 1, var_t };
        vector<RetVal> then = {};
        MethodProfile method = { func, retVal, then, 0 };
        _methods.push_back(method);
        // TODO swith _returnTypes to _methods
        std::map<const char *, any> returnMap { { func, var_t } };
        _returnTypes.push_back(returnMap);
        return *this;
    }

    /**
     * @brief Edit the defaualt behaviour of how many times a mocked method will return
     * the value. The default value for the (int)RetVal.first is 1. If the next vector
     * is empty the mock method will always return (std:any)RetVal.second stored 
     * in MethodProfile::retVal. If MethodProfile::then has elements the
     * mocked method will return the next value, decrementing the value
     * integer n stored in the lowest index of MethodProfile::then.
     * If n reaches 0 and a subsequent element is present in   
     * MethodProfile::then, the process will be repeated
     * If no more elements are present to use the mock
     * methods will keep on returning the last value
     * rather than generate an exception due to
     * a failure to return the correct type.
     *    
     * @param n int
     * @return Emulator& 
     */
    Emulator& times(int n) {     
        for (auto method_It = _methods.begin(); method_It != _methods.end(); ++method_It) {
            if (method_It->methodName == _lastFunc) {
                if (method_It->then.size() == 0) {
                    method_It->retVal.first = n;
                } else {
                    RetVal retVal = method_It->then.back();
                    retVal.first = n;
                }
                break;
            }
        }
        return *this;
    }

    /**
     * @brief Create another RetVal instance to store within an existing
     * MethodProfile. The returns() method must be called first as
     * _lastFunc is used to locate the correct MethodProfile.
     * If _lastFunc is empty a EmulatorNoProfileException is
     * thrown to prompt the user to chain calls correctly.
     * 
     * @param var_t std::any
     * @return Emulator& 
     * @throw EmulatorNoProfileException
     */
    Emulator& then(any var_t) {
        for (auto method_It = _methods.begin(); method_It != _methods.end(); ++method_It) {
            if (method_It->methodName == _lastFunc) {
                RetVal retVal = { 1, var_t };
                method_It->then.push_back(retVal);
                break;
            }
        }
        return *this; 
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
        // TODO move into Invokable
        // TODO change std::map to std::pair
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

    // /**
    //  * @brief After setting the invokable method object
    //  * it is stored in the _methods vector of this
    //  * instance.
    //  * 
    //  * @param func const char *
    //  * @param function pointer
    //  */
    // void setMethod(const char * methodName, void (*method)(), std::any var_t) {
    //     std::map<int, std::any> retVal = { { 1, var_t } };
    //     MethodProfile invokableMethod = { methodName, method, retVal };
    //     _methods.push_back(invokableMethod);
    // }

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
        string logMsg = string("Entered mock method for ") + string(func);
        LOG_2_TXT(logMsg.c_str());
        await();
        int exception = throwException(func);
        if (exception > -1) {
            LOG_2_TXT("Throwing expected exception");
            throw exception;
        }
        LOG_2_TXT("Calling doReturn method");
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
        // TODO refactor to use and edit vector of invokables, edit the remaining calls during runtime
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
            LOG_2_TXT("In doReturn method, return value not found. Throwing NoReturnValueException");
            setInternalException(PSUEDO_EXCEPTION_NO_RET_VAL);
            throw PSUEDO_EXCEPTION_NO_RET_VAL;
        }
        LOG_2_TXT("Returning expected value from doReturn method");
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

    std::string _lastFunc = "";

    /**
     * @brief A store of methods for the mock class.
     * Methods are stored as a vector of 
     * function pointers.
     * 
     */
    vector<MethodProfile> _methods;

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
