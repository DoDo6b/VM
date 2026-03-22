#include "../operations.h"


void opCALL (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    const char* retaddr = (vm->codeseg.rip + sizeof (opcode_t) + sizeof (offset_t));

    if (vm->codeseg.code > retaddr || retaddr > vm->codeseg.code + vm->codeseg.size)
    {
        ErrAcc |= VM_ERRCODE (VM_SEGFAULT);
        log_err ("runtime error", "segfault");
        return;
    }
    
    stackPush (vm->callstack, &retaddr);

    return opJMP (vm);
}

void opRET (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    const char* retaddr = 0;
    stackPop (vm->callstack, &retaddr);

    if (vm->codeseg.code > retaddr || retaddr > vm->codeseg.code + vm->codeseg.size)
    {
        ErrAcc |= VM_ERRCODE (VM_SEGFAULT);
        log_err ("runtime error", "segfault");
        return;
    }

    vm->codeseg.rip = retaddr;

    return;
}