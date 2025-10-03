#ifndef POLICY_H
#define POLICY_H

#define AREALLOC

#define TOLERANCEPOLICY_LVL 0
#define SECURITY_LVL        1
#define LOGGINGLVL          3


#if SECURITY_LVL >= 1
    #define S1(...) __VA_ARGS__
#else
    #define S1(...)
#endif


#if TOLERANCEPOLICY_LVL >= 1
    #define T1(...) __VA_ARGS__
#else
    #define T1(...)
#endif

#if TOLERANCEPOLICY_LVL >= 2
    #define T2(...) __VA_ARGS__
#else
    #define T2(...)
#endif

#if TOLERANCEPOLICY_LVL >= 3
    #define T3(...) __VA_ARGS__
#else
    #define T3(...)
#endif


#if LOGGINGLVL >= 1
    #define L1(...) __VA_ARGS__
#else
    #define L1(...)
#endif

#if LOGGINGLVL >= 2
    #define L2(...) __VA_ARGS__
#else
    #define L2(...)
#endif

#if LOGGINGLVL >= 3
    #define L3(...) __VA_ARGS__
#else
    #define L3(...)
#endif


#endif