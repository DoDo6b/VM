#ifndef BUFCODES_H
#define BUFCODES_H


typedef enum
{
    BUFNULL      = 1,
    BUFNOTINITED = 2,
    BUFDETACHED  = 4,
    BUFFACCESS   = 8,
    BUFOVERFLOW  = 16,
    BUFSTREAM    = 32,
    BUFSYNTAX    = 64,

}BUFEC;


#endif