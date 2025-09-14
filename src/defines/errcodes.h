#ifndef ERRCODES_H
#define ERRCODES_H


typedef enum
{
    CANTOPEN            = 1,
    NULLRECEIVED        = 2,
    SYNTAX              = 4,
    WRONGVERSION        = 8,
    OPCODENOTFOUND      = 16,
    MISSINGOPERAND      = 32,
}EC;


#endif