#include "../operations.h"


void opJMP (VM* vm)
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

#define CONDJMP(name, condition) \
    void op ## name (VM* vm)\
    {\
        assertStrict (VMVerify (vm) == 0, "vm corrupted");\
        \
        if (condition) opJMP (vm);\
        else vm->codeseg.rip += sizeof (offset_t) + sizeof (opcode_t);\
        return;\
    }

CONDJMP (JNZ, !VMZF)
CONDJMP (JZ,   VMZF)
CONDJMP (JL,  !VMZF &&  VMCF)
CONDJMP (JG,  !VMZF && !VMCF)
CONDJMP (JLE,  VMZF ||  VMCF)
CONDJMP (JGE,  VMZF || !VMCF)

#undef COND_JMP