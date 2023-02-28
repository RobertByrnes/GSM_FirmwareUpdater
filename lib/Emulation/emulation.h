#ifndef EMULATION_FRAME_H
#define EMULATION_FRAME_H

#include <unity.h>

#if not defined(ARDUINO)
#include <ArduinoFake.h>
#define log_i(...) 
#define log_w(...)
#define log_d(...) 
#define log_e(...) 
#define vTaskDelay(...)
#endif

#endif
