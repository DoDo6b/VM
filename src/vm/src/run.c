#include "vm.h"
#include "operations/operations.h"

#include "../run.h"


static Erracc_t runThread (const char* bcname, size_t stackSiz, size_t ramSiz)
{
    VM* vm = VMInit (bcname, stackSiz, ramSiz);
    if (!vm)
    {
        log_err ("init error", "vm wasnt initialized");
        return ErrAcc;
    }

    size_t instrc = 0;
    bool halt = false;
    while (!halt)
    {
        instrc++;

        if (*vm->codeseg.rip == HALT) break;

        if ((unsigned char)*vm->codeseg.rip > NUM_OPS)
        {
            ErrAcc |= VM_ERRCODE (VM_OPCODENOTFOUND);
            log_srcerr (
                bcname,
                instrc,
                "syntax error",
                "operation not found: \"%0X\"",
                *vm->codeseg.rip
            );
            VMdump (vm);
            VMFree (vm);
            return ErrAcc;
        }

        operations[(unsigned char)*vm->codeseg.rip].exec (vm);
        
        if (ErrAcc)
        {
            log_err ("runtime error", "aborting");
            VMdump (vm);
            break;
        }
    }

    VMFree (vm);
    return ErrAcc;
}


Erracc_t run (const char* input, size_t stackSiz, size_t ramSiz)
{
    log_string ("<grn>starting...<dft>\n");

    if (runThread (input, stackSiz, ramSiz) == 0) log_string ("<grn>Work is done<dft>\n");

    return ErrAcc;
}