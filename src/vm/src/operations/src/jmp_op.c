#include "../operations.h"


void op_JMP (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    vm->codeseg.rip += sizeof (opcode_t);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
    offset_t offset = *(const offset_t*)vm->codeseg.rip;
#pragma GCC diagnostic pop
    vm->codeseg.rip += offset;

    if (vm->codeseg.code > vm->codeseg.rip || vm->codeseg.rip > vm->codeseg.code + vm->codeseg.size)
    {
        ErrAcc |= VM_ERRCODE (VM_SEGFAULT);
        log_err ("runtime error", "segfault");
        return;
    }


    return;
}

#define VMZF  ((vm->rflags >> 6) & 1ULL)
#define VMCF  ( vm->rflags       & 1ULL)

#define COND_JMP(name, condition) \
    void op_ ## name (VM* vm)\
    {\
        assertStrict (VMVerify (vm) == 0, "vm corrupted");\
        \
        if (condition) op_JMP (vm);\
        else vm->codeseg.rip += sizeof (offset_t) + sizeof (opcode_t);\
        return;\
    }

COND_JMP (JNZ, !VMZF)
COND_JMP (JZ,   VMZF)
COND_JMP (JL,  !VMZF &&  VMCF)
COND_JMP (JG,  !VMZF && !VMCF)
COND_JMP (JLE,  VMZF ||  VMCF)
COND_JMP (JGE,  VMZF || !VMCF)

#undef COND_JMP