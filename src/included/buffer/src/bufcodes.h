#ifndef BUFCODES_H
#define BUFCODES_H


typedef enum
{
    BUFNULL      = 2048,
    BUFNOTINITED = 4096,
    BUFDETACHED  = 8192,
    BUFFACCESS   = 16384,
    BUFOVERFLOW  = 32768,
    BUFSTREAM    = 65536,
    BUFSYNTAX    = 131072,

}BUFEC;


#endif