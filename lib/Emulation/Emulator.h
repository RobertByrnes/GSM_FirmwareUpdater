#pragma once

// TODO move to package
#if not defined(EMULATOR_H)
#define EMULATOR_H

#include <string>
#include <vector>
#include <map>
#include <any>
#include <EmulationInterface.h>
#include <Exceptions/NoReturnValueException.h>
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
#define EMULATION_LOG(x)                               log_out<const char *>(x);
#else 
#define EMULATION_LOG(x)
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

    void dumpMethods() {
        for (auto method_It = _methods.begin(); method_It != _methods.end(); ++method_It) {
            
        }
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
        // TODO check if method already exists? or allow user to add again but will never be actioned?
        _lastFunc = func;
        RetVal retVal = { 0, var_t };
        vector<RetVal> then = {};
        MethodProfile method = { func, retVal, then, 0 };
        _methods.push_back(method);
        return *this;
    }

    /**
     * @brief Edit the defaualt behaviour of how many times a mocked method will return
     * the value. The default value for the (int)RetVal.first is 0. If the next vector
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
        _methods.clear();
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
        EMULATION_LOG(logMsg.c_str());
        await();
        int exception = throwException(func);
        if (exception > -1) {
            EMULATION_LOG("Throwing expected exception");
            throw exception;
        }
        EMULATION_LOG("Calling doReturn method");
        return doReturn<T>(func);
    }

    /**
     * @brief Search the return values vector for the function name,
     * if found will return the return value of the return type.
     * The logic is implemented through findRetVal().
     * 
     * @details In MethodProfile.RetVal is n (first) > 0? If yes
     * decrement n and return the value (second).
     * 
     * @details If MethodProfile.then is empty and MethodProfile.RetVal
     * n (first) is 0 return the value (second) anyway.
     * 
     * @details If MethodProfile.then not empty and MethodProfile.RetVal
     * n (first) is > 0 return the value (second) and decrement n.
     * 
     * @details If MethodProfile.next size() > 0 set MethodProfile.RetVal
     * to copy the first element of MethodProfile.next, decrement n and
     * return value(of RetVal) and remove the first element of next.
     * 
     * @tparam T typename
     * @param func const char *
     * @return T typename
     * @throws NoReturnValueException
     */
    template<typename T>
    T doReturn(const char * func) { 
        T value;
        bool canReturn = false;
        for (auto method_It = _methods.begin(); method_It != _methods.end(); ++method_It) {
            if (method_It->methodName == func) {
                value = this->findRetVal<T>(method_It);
                method_It->invoked += 1;
                break;
            }
        }
        return value;
    }

    /**
     * @brief Implements the logical search and is called by doReturn().
     * Step 1. In RetVal is n > 0? If yes decrement n and return value.
     * Step 2. If RetVal.n is 0 Check MethodProfile.next for size() if > 0 then not empty.
     * Step 3. If MethodProfile.next size() is 0 return RetVal.
     * Step 4. If MethodProfile.next size() > 0 set retVal to the first element of next, 
     * decrement n and return value(of retVal) and remove the first element of next.
     * 
     * @tparam T 
     * @param method_It 
     * @return T 
     * @throws NoReturnValueException
     */
    template<typename T>
    T findRetVal(std::vector<MethodProfile>::iterator &method_It) {
        T value;
        // if n > 0 return and decrement n
        if (method_It->retVal.first > 0) {
            --method_It->retVal.first;
            try {
                value = std::any_cast<T>(method_It->retVal.second); 
            } catch (std::bad_any_cast e) {
                EMULATION_LOG(e.what());
            }
        }
        // answer if then vector contains elements 
        bool thenIsEmpty = false;
        if (method_It->then.size() == 0) {
            thenIsEmpty = true;
        }
        if (method_It->retVal.first == 0) {
            // if then is empty and n is 0 return value anyway
            if (thenIsEmpty) {            
                try {
                    value = std::any_cast<T>(method_It->retVal.second); 
                    return value;
                } catch (std::bad_any_cast e) {
                    std::string eMessage = "Return value for " + std::string(method_It->methodName) + " found, casting failed: " + e.what();
                    EMULATION_LOG(eMessage.c_str());
                    NoReturnValueException(eMessage.c_str());
                }
            } else { // if then not empty and n is 0 move onto to then
                method_It->retVal = method_It->then[0];
                method_It->then.erase(method_It->then.begin()); 
                this->findRetVal<T>(method_It);
            }
        }
        std::string eMessage = "Could not find a return value for " + std::string(method_It->methodName) + ", .returns() must be called for this method.";
        NoReturnValueException(eMessage.c_str());
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
};

#endif
