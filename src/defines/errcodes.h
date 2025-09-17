#ifndef ERRCODES_H
#define ERRCODES_H


typedef enum
{
    CANTOPEN            = 1,
    NULLRECEIVED        = 2,
    SYNTAX              = 4,
    WRONGVERSION        = 8,
    OPCODENOTFOUND      = 16,
    WRITINGERROR        = 32,
    VMINITIALIZATION    = 64,
    STACKVERIFICATION   = 128,
    VMSELFTESTING       = 256,
    MISSINGOPERAND      = 512,
    BYTECODECORRUPTED   = 1024,
}EC;


#endif