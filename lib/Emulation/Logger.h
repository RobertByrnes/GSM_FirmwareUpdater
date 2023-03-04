// TODO move to package
#pragma once

#include <iostream>
#include <fstream>

using namespace std;

ofstream logFile ("emulation.log");


template<typename T>
void log_out(T output) { 
    logFile << output << endl;
}