#ifndef KASSERT_H
#define KASSERT_H


#include <stdlib.h>
#include <stdio.h>
#include "included/logger/logger.h"
#include <stdbool.h>

#define CALLATEXIT


void assertStrict_  (const char* callerFile, unsigned int callerLine, const char* callerFunc, const char* conditionStr, bool condition, const char* dbgMsg);
bool assertSoft_    (const char* callerFile, unsigned int callerLine, const char* callerFunc, const char* conditionStr, bool condition, const char* dbgMsg);

#ifndef NDEBUG

#define assertStrict(condition, dbgMsg)    assertStrict_   (__FILE__, __LINE__, __func__, #condition, condition, dbgMsg)
#define assertSoft(condition, dbgMsg)      assertSoft_     (__FILE__, __LINE__, __func__, #condition, condition, dbgMsg)

#define assertStrict_auto(condition, dbgMsg)    assertStrict_   (__FILE__, __LINE__, __func__, #condition, condition, dbgMsg)
#define assertSoft_auto(condition, dbgMsg)      assertSoft_     (__FILE__, __LINE__, __func__, #condition, condition, dbgMsg)

#else

#define assertStrict(condition, dbgMsg)
#define assertSoft(condition, dbgMsg)

#define assertStrict_auto(condition, dbgMsg)    condition
#define assertSoft_auto(condition, dbgMsg)      condition

#endif

#define assertStrict_NDBG(condition, dbgMsg)    assertStrict_   (__FILE__, __LINE__, __func__, #condition, condition, dbgMsg)
#define assertSoft_NDBG(condition, dbgMsg)      assertSoft_     (__FILE__, __LINE__, __func__, #condition, condition, dbgMsg)


#endif