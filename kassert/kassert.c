#include "kassert.h"


void assertStrict_  (const char* callerFile, unsigned int callerLine, const char* callerFunc, const char* conditionStr, bool condition, const char* dbgMsg)
{
    if (!condition)
    {
        log_string
        (
            "%s:%d: %s: <b><red>strict assertion failed:<dft> %s</b>\n",
            callerFile,
            callerLine,
            callerFunc,
            dbgMsg ? dbgMsg : "<i>no debug message</i>"
        );

        log_string
        (
            "   %d | %s\n",
            callerLine,
            conditionStr
        );

        #ifdef CALLATEXIT
        exit(EXIT_FAILURE);
        #else
        abort();
        #endif
    }
}

bool assertSoft_  (const char* callerFile, unsigned int callerLine, const char* callerFunc, const char* conditionStr, bool condition, const char* dbgMsg)
{
    if (!condition)
    {
        log_string
        (
            "%s:%d: %s: <b><red>soft assertion failed:<dft> %s</b>\n",
            callerFile,
            callerLine,
            callerFunc,
            dbgMsg ? dbgMsg : "<i>no debug message</i>"
        );
        log_string
        (
            "   %d | %s\n",
            callerLine,
            conditionStr
        );
        
        return false;
    }
    return true;
}