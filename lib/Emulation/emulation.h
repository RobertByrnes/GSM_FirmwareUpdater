#ifndef EMULATION_FRAME_H
#define EMULATION_FRAME_H

#include <Arduino.h>
#include <unity.h>
#include <ctime>

// TODO remove from here if ArduinoFake is replaced entirely
// Cover ourselves for the millis function as the ArduinoFake fails in this case
// std::clock_t EMULATION_START = std::clock();
unsigned long _milliseconds = 1000;
unsigned long milliseconds() { _milliseconds += _milliseconds; return _milliseconds; }
// unsigned long milliseconds() { return std::clock() - EMULATION_START / (double)(CLOCKS_PER_SEC / 1000); }

#if not defined(ARDUINO)
#define log_i(...) 
#define log_w(...)
#define log_d(...) 
#define log_e(...) 
#define vTaskDelay(...)
#define millis()                                    milliseconds()
#else
#define millis()                                    milliseconds()
#endif

#endif
