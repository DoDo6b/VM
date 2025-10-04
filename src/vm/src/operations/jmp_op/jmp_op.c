#include "jmp_op.h"


Erracc_t jmp (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    vm->codeseg.rip += sizeof (opcode_t);
    
    offset_t offset = *(const offset_t*)vm->codeseg.rip;
    vm->codeseg.rip += offset;

    if (vm->codeseg.code > vm->codeseg.rip || vm->codeseg.rip > vm->codeseg.code + vm->codeseg.size)
    {
        ErrAcc |= VM_ERRCODE (VM_SEGFAULT);
        log_err ("runtime error", "segfault");
        return ErrAcc;
    }


    return ErrAcc;
}

Erracc_t jnz (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    if (!vm->zf) jmp (vm);
    else vm->codeseg.rip += sizeof (offset_t) + sizeof (opcode_t);
    return ErrAcc;
}

Erracc_t jz (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");
    
    if (vm->zf) jmp (vm);
    else vm->codeseg.rip += sizeof (offset_t) + sizeof (opcode_t);
    return ErrAcc;
}

Erracc_t jl (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");
    
    if (!vm->zf && vm->cf) jmp (vm);
    else vm->codeseg.rip += sizeof (offset_t) + sizeof (opcode_t);
    return ErrAcc;
}

Erracc_t jg (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");
    
    if (!vm->zf && !vm->cf) jmp (vm);
    else vm->codeseg.rip += sizeof (offset_t) + sizeof (opcode_t);
    return ErrAcc;
}

Erracc_t jle (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");
    if (vm->zf || vm->cf) jmp (vm);
    else vm->codeseg.rip += sizeof (offset_t) + sizeof (opcode_t);
    return ErrAcc;
}

Erracc_t jge (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");
    
    if (vm->zf || !vm->cf) jmp (vm);
    else vm->codeseg.rip += sizeof (offset_t) + sizeof (opcode_t);
    return ErrAcc;
}