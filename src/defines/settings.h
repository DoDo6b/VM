#ifndef SETTINGS_H
#define SETTINGS_H


#define IMMEDIATESTOP

#ifdef IMMEDIATESTOP
    #define IMSTP(...)  __VA_ARGS__
#else
    #define IMSTP(...)
#endif


#endif