#ifndef TRANSLATOR_H
#define TRANSLATOR_H


#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "../included/logger/logger.h"
#include "../included/kassert/kassert.h"
#include "../included/buffer/buffer.h"
#include "../defines/arch.h"
#include "src/translatorcodes.h"
#include "src/instructions/instructions.h"


#define BUFFERSIZE BUFSIZ

void instrinit ();

Erracc_t translate (const char* input, const char* output);


#endif