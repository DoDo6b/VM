#ifndef ANIMAGEN_H
#define ANIMAGEN_H


#include "../../included/logger/logger.h"
#include "../../included/kassert/kassert.h"
#include "../../included/buffer/buffer.h"

#define PUSH "PUSH "
#define MOV  "MOV "
#define DRAW "DRAW\n"
#define HALT "HALT\n"

size_t animationGen (const char* gif);


#endif