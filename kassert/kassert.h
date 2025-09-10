#ifndef KASSERT_H
#define KASSERT_H


#include <stdlib.h>
#include <stdio.h>
#include "../logger/logger.h"
#include <stdbool.h>

#define CALLATEXIT


void assertStrict_  (const char* callerFile, unsigned int callerLine, const char* callerFunc, const char* conditionStr, bool condition, const char* dbgMsg);
bool assertSoft_    (const char* callerFile, unsigned int callerLine, const char* callerFunc, const char* conditionStr, bool condition, const char* dbgMsg);

#ifndef NDEBUG

#define assertStrict(condition, dbgMsg)    assertStrict_   (__FILE__, __LINE__, __func__, #condition, condition, dbgMsg)
#define assertSoft(condition, dbgMsg)      assertSoft_     (__FILE__, __LINE__, __func__, #condition, condition, dbgMsg)

#else

#define assertStrict(condition, dbgMsg)
#define assertSoft(condition, dbgMsg)

#endif

#define assertStrict_NDBG(condition, dbgMsg)    assertStrict_   (__FILE__, __LINE__, __func__, #condition, condition, dbgMsg)
#define assertSoft_NDBG(condition, dbgMsg)      assertSoft_     (__FILE__, __LINE__, __func__, #condition, condition, dbgMsg)


#endif