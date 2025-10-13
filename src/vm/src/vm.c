#include "vm.h"


VM* VMInit (const char* bcname, size_t stackSize, size_t ramSize)
{
    assertStrict (bcname, "received NULL");

    VM* vm = (VM*)calloc(1, sizeof (VM));
    if (!vm)
    {
        ErrAcc |= VM_ERRCODE ((VM_ALLOCERR | VM_ERRONINIT));
        log_err ("allocation error", "calloc returned NULL");
        return NULL;
    }

    if (buildCodeseg (&vm->codeseg, bcname))
    {
        ErrAcc |= VM_ERRCODE (VM_ERRONINIT);
        log_err ("init error", "code segment wasnt initialized");
        return NULL;
    }

    if (buildRAMseg (&vm->memseg, ramSize))
    {
        ErrAcc |= VM_ERRCODE (VM_ERRONINIT);
        log_err ("init error", "memory segment wasnt initialized");
        return NULL;
    }
    vm->stack = stackInit (stackSize, sizeof (operand_t));
    if (ErrAcc)
    {
        ErrAcc |= VM_ERRCODE (VM_ERRONINIT);
        log_err ("init error", "stack segment wasnt initialized");
        return NULL;
    }

    vm->callstack = stackInit (stackSize, sizeof (const char*));
    if (ErrAcc)
    {
        ErrAcc |= VM_ERRCODE (VM_ERRONINIT);
        log_err ("init error", "call stack segment wasnt initialized");
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
        stackFree   ( vm->callstack);
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
        ErrAcc |= VM_ERRCODE (VM_CODESEGERR);
        log_err ("verification error", "code segment corrupted");
    }

    if (RAMsegverify (&vm->memseg))
    {
        ErrAcc |= VM_ERRCODE (VM_RAMERR);
        log_err ("verification error", "ram segment corrupted");
    }

    Erracc_t stackST = stackVerify (vm->stack);
    if (stackST != 0)
    {
        ErrAcc |= VM_ERRCODE (VM_STACKERR);
        log_err ("verification error", "stack failed verification with code: %llu", stackST);
    }

    Erracc_t callStackST = stackVerify (vm->callstack);
    if (callStackST != 0)
    {
        ErrAcc |= VM_ERRCODE (VM_STACKERR);
        log_err ("verification error", "call stack failed verification with code: %llu", callStackST);
    }

    return ErrAcc;
}

Erracc_t VMdump (const VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    log_string ("<blu><b>Virtual Machine dump:</b><dft>\n");

    log_string
    (
        "<blu>gp registers:<dft>\n{\n"
        "  rax: %lld\n"
        "  rcx: %lld\n"
        "  rdx: %lld\n"
        "  rbx: %lld\n"
        "  rsp: %lld\n"
        "  rbp: %lld\n"
        "  rsi: %lld\n"
        "  rdi: %lld\n"
        "}\n",
        vm->rax,
        vm->rcx,
        vm->rdx,
        vm->rbx,
        vm->rsp,
        vm->rbp,
        vm->rsi,
        vm->rdi
    );
    log_string ("rflags: ");
    for (size_t i = 0; i < sizeof (vm->rflags) * 8; i++) log_string ("%d", (vm->rflags >> i) & 1ULL);
    log_string ("\n");

    codesegDump (&vm->codeseg);
    stackDump   (vm->stack);
    stackDump   (vm->callstack);
    RAMdump     (&vm->memseg);

    return ErrAcc;
}