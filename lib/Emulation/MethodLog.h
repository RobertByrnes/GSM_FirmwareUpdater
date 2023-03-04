#ifndef METHOD_LOG_H
#define METHOD_LOG_H

#include <iostream>
#include <fstream>
#include <vector>
#include "MethodProfile.h"

using namespace std;

class MethodLog {
public:
    ofstream _file;

    MethodLog() {}
    ~MethodLog() {}

    void dumpMethodProfiles(vector<MethodProfile> _methods, const char * filename="method") {
        std::string fileName = std::string(filename) + ".log";
        _file.open(fileName.c_str());
        for (auto method_It = _methods.begin(); method_It != _methods.end(); ++method_It) {
            std::string time = (method_It->invoked == 1) ? "time" : "times"; 
            _file << "Method: " << method_It->methodName << "() [invoked " << method_It->invoked << " " << time << "] ";
            size_t nRetVals = method_It->then.size() + 1;
            std::string value = (nRetVals == 1) ? "value" : "values"; 
            _file << "with " << nRetVals << " return " << value << endl;
        }
    }
};

#endif
