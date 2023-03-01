#ifndef EMULATION_FRAME_H
#define EMULATION_FRAME_H

#include <Arduino.h>
#include <unity.h>

#if not defined(ARDUINO)
#define log_i(...) 
#define log_w(...)
#define log_d(...) 
#define log_e(...) 
#define vTaskDelay(...)
#endif

#endif
