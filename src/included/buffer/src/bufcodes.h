#ifndef BUFCODES_H
#define BUFCODES_H


#include "../../Macro.h"

#define BUFEC_SHIFT 16

#define BUF_ERRCODE(ec)  BUILD_ERRCODE (ec, BUFEC_SHIFT)

typedef enum
{
    BUFNULL,
    BUFNOTINITED,
    BUFDETACHED,
    BUFFACCESS,
    BUFOVERFLOW,
    BUFSTREAM,
    BUFSYNTAX,

}BUFEC;


#endif