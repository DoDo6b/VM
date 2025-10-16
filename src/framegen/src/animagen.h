#ifndef ANIMAGEN_H
#define ANIMAGEN_H


#include "../../included/logger/logger.h"
#include "../../included/kassert/kassert.h"
#include "../../included/buffer/buffer.h"

#define PUSH "push "
#define MOV  "mov "
#define DRAW "draw\n"
#define HALT "halt\n"

size_t animationGen (const char* gif, int w, int h);


#endif