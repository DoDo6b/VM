#ifndef SETTINGS_H
#define SETTINGS_H

#define LOG 2

#if LOG >= 1
    #define LOG1(...)  log_string (__VA_ARGS__)
#else
    #define LOG1(...)
#endif

#if LOG >= 2
    #define LOG2(...)  log_string (__VA_ARGS__)
#else
    #define LOG2(...)
#endif

#endif