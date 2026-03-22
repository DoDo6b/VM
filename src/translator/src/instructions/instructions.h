#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H


#include <inttypes.h>
#include <ctype.h>

#include "../../../included/logger/logger.h"
#include "../../../included/kassert/kassert.h"
#include "../../../included/buffer/buffer.h"
#include "../../../defines/arch.h"
#include "../translatorcodes.h"
#include "operands/operands.h"


void handlePUSH (Buffer* bufR, Buffer* bufW);
void handleMOV  (Buffer* bufR, Buffer* bufW);

void handleHALT (Buffer* FICTIVE, Buffer* bufW);
void handleIN   (Buffer* FICTIVE, Buffer* bufW);
void handleOUT  (Buffer* FICTIVE, Buffer* bufW);
void handlePOP  (Buffer* FICTIVE, Buffer* bufW);
void handleCMP  (Buffer* FICTIVE, Buffer* bufW);
void handleADD  (Buffer* FICTIVE, Buffer* bufW);
void handleSUB  (Buffer* FICTIVE, Buffer* bufW);
void handleMUL  (Buffer* FICTIVE, Buffer* bufW);
void handleDIV  (Buffer* FICTIVE, Buffer* bufW);
void handleDRAW (Buffer* FICTIVE, Buffer* bufW);
void handleDMP  (Buffer* FICTIVE, Buffer* bufW);
void handleRET  (Buffer* FICTIVE, Buffer* bufW);

void handleJMP  (Buffer* bufR, Buffer* bufW);
void handleJNZ  (Buffer* bufR, Buffer* bufW);
void handleJZ   (Buffer* bufR, Buffer* bufW);
void handleJL   (Buffer* bufR, Buffer* bufW);
void handleJLE  (Buffer* bufR, Buffer* bufW);
void handleJG   (Buffer* bufR, Buffer* bufW);
void handleJGE  (Buffer* bufR, Buffer* bufW);
void handleCALL (Buffer* bufR, Buffer* bufW);

void labelDecl  (const char* str, Buffer* bufW);


void jmpWLdump ();
size_t remUnmngldJMP ();


#endif