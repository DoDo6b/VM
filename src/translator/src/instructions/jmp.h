#ifndef JMP_H
#define JMP_H


#include "../../../included/logger/logger.h"
#include "../../../included/kassert/kassert.h"
#include "../../../included/buffer/buffer.h"
#include "../../../defines/arch.h"
#include "../translatorcodes.h"
#include "operands/operands.h"


void labelDecl  (const char* str, Buffer* bufW);

void handleJMP  (Buffer* bufR, Buffer* bufW);
void handleJNZ  (Buffer* bufR, Buffer* bufW);
void handleJZ   (Buffer* bufR, Buffer* bufW);
void handleJL   (Buffer* bufR, Buffer* bufW);
void handleJLE  (Buffer* bufR, Buffer* bufW);
void handleJG   (Buffer* bufR, Buffer* bufW);
void handleJGE  (Buffer* bufR, Buffer* bufW);
void handleCALL (Buffer* bufR, Buffer* bufW);


void jmpWLdump ();
size_t remUnmngldJMP ();


#endif