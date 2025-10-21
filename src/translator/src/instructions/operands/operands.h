#ifndef OPERANDS_H
#define OPERANDS_H


#include <ctype.h>

#include "../../../../included/logger/logger.h"
#include "../../../../included/kassert/kassert.h"
#include "../../../../included/buffer/buffer.h"
#include "../../../../defines/arch.h"
#include "../../translatorcodes.h"

void reginit ();

operand_t getImm      (Buffer* bufR);
opcode_t  getReg      (Buffer* bufR);
Erracc_t  tokBrackets (Buffer* bufR, opcode_t* reg, offset_t* offset);

#endif