#include "../operations.h"


Erracc_t call (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    const char* retaddr = (vm->codeseg.rip + sizeof (opcode_t) + sizeof (offset_t));

    if (vm->codeseg.code > retaddr || retaddr > vm->codeseg.code + vm->codeseg.size)
    {
        VMdump (vm);
        ErrAcc |= VM_ERRCODE (VM_SEGFAULT);
        log_err ("runtime error", "segfault");
        return ErrAcc;
    }
    
    stackPush (vm->callstack, &retaddr);

    return jmp (vm);
}

Erracc_t ret (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    const char* retaddr = 0;
    stackPop (vm->callstack, &retaddr);

    if (vm->codeseg.code > retaddr || retaddr > vm->codeseg.code + vm->codeseg.size)
    {
        VMdump (vm);
        ErrAcc |= VM_ERRCODE (VM_SEGFAULT);
        log_err ("runtime error", "segfault");
        return ErrAcc;
    }

    vm->codeseg.rip = retaddr;

    return ErrAcc;
}