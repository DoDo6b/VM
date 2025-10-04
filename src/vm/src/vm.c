#include "vm.h"


VM* VMInit (const char* bcname, size_t stackSize, size_t ramSize)
{
    assertStrict (bcname, "received NULL");

    VM* vm = (VM*)calloc(1, sizeof (VM));
    if (!vm)
    {
        ErrAcc |= VM_ERRCODE (VM_ERRONINIT);
        log_err ("internal error", "calloc returned NULL");
        return NULL;
    }

    if (buildCodeseg (&vm->codeseg, bcname))
    {
        ErrAcc |= VM_ERRCODE (VM_ERRONINIT);
        log_err ("internal error", "code segment wasnt initialized");
        return NULL;
    }

    if (buildRAMseg (&vm->memseg, ramSize))
    {
        ErrAcc |= VM_ERRCODE (VM_ERRONINIT);
        log_err ("internal error", "memory segment wasnt initialized");
        return NULL;
    }
    vm->stack = stackInit (stackSize, sizeof (operand_t));
    if (ErrAcc)
    {
        ErrAcc |= VM_ERRCODE (VM_ERRONINIT);
        log_err ("internal error", "stack segment wasnt initialized");
        return NULL;
    }

    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    return vm;
}

void VMFree (VM* vm)
{
    if (vm)
    {
        freeCodeseg (&vm->codeseg);
        freeRAMseg  (&vm->memseg);
        stackFree   ( vm->stack);
        memset (vm, 0, sizeof (VM));
        free (vm);
    }
}

Erracc_t VMVerify (const VM* vm)
{
    if (vm == NULL)
    {
        ErrAcc |= VM_ERRCODE (VM_NULLRECEIVED);
        log_err ("verification error", "received NULL");
        return ErrAcc;
    }

    bool ax = &vm->rax == &vm->regs[0];
    bool cx = &vm->rcx == &vm->regs[1];
    bool dx = &vm->rdx == &vm->regs[2];
    bool bx = &vm->rbx == &vm->regs[3];
    bool sp = &vm->rsp == &vm->regs[4];
    bool bp = &vm->rbp == &vm->regs[5];
    bool si = &vm->rsi == &vm->regs[6];
    bool di = &vm->rdi == &vm->regs[7];

    if(!ax | !cx | !dx | !bx | !sp | !bp | !si | !di)
    {
        ErrAcc |= VM_ERRCODE (VM_REGMISSADDRESSING);
        log_err ("verification error", "discrepancy in the addresses");
    }

    if (codesegVerify (&vm->codeseg))
    {
        ErrAcc |= VM_ERRCODE (VM_CODESEGVERIFICATION);
        log_err ("verification error", "code segment corrupted");
    }

    if (RAMsegverify (&vm->memseg))
    {
        ErrAcc |= VM_ERRCODE (VM_RAMVERIFICATION);
        log_err ("verification error", "ram segment corrupted");
    }

    Erracc_t stackST = stackVerify (vm->stack);
    if (stackVerify (vm->stack) != 0)
    {
        ErrAcc |= VM_ERRCODE (VM_STACKVERIFICATION);
        log_err ("verification error", "stack failed verification with code: %llu", stackST);
    }

    return ErrAcc;
}