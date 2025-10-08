#include "../operations.h"


Erracc_t jmp (VM* vm)
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
        VMdump (vm);
        ErrAcc |= VM_ERRCODE (VM_SEGFAULT);
        log_err ("runtime error", "segfault");
        return ErrAcc;
    }


    return ErrAcc;
}

#define VMZF  ((vm->rflags >> 6) & 1ULL)
#define VMCF  ( vm->rflags       & 1ULL)

Erracc_t jnz (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    if (!VMZF) jmp (vm);
    else vm->codeseg.rip += sizeof (offset_t) + sizeof (opcode_t);
    return ErrAcc;
}

Erracc_t jz (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");
    
    if (VMZF) jmp (vm);
    else vm->codeseg.rip += sizeof (offset_t) + sizeof (opcode_t);
    return ErrAcc;
}

Erracc_t jl (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");
    
    if (!VMZF && VMCF) jmp (vm);
    else vm->codeseg.rip += sizeof (offset_t) + sizeof (opcode_t);
    return ErrAcc;
}

Erracc_t jg (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");
    
    if (!VMZF && !VMCF) jmp (vm);
    else vm->codeseg.rip += sizeof (offset_t) + sizeof (opcode_t);
    return ErrAcc;
}

Erracc_t jle (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");
    if (VMZF || VMCF) jmp (vm);
    else vm->codeseg.rip += sizeof (offset_t) + sizeof (opcode_t);
    return ErrAcc;
}

Erracc_t jge (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");
    
    if (VMZF || !VMCF) jmp (vm);
    else vm->codeseg.rip += sizeof (offset_t) + sizeof (opcode_t);
    return ErrAcc;
}