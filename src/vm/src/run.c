#include "vm.h"
#include "operations/arithmetic_op/arithmetic.h"
#include "operations/stack_op/stack_op.h"
#include "operations/jmp_op/jmp_op.h"
#include "operations/reg_op/reg_op.h"

#include "../run.h"


#define STOPONERR(func)  func;\
if (ErrAcc != 0)\
{\
    ErrAcc |= VM_ERRCODE (VM_RUNTIMEERR);\
    log_err ("runtime error", "run has stopped with code: %llu", ErrAcc);\
    VMFree (vm);\
    return ErrAcc;\
}

static Erracc_t runThread (const char* bcname)
{
    VM* vm = VMInit (bcname, STACKSIZE, RAMSIZE);
    if (!vm)
    {
        log_err ("internal error", "vm wasnt initialized");
        return ErrAcc;
    }

    size_t instrc = 0;
    bool halt = false;
    while (!halt)
    {
        instrc++;

        switch (*vm->codeseg.rip)
        {
            case OUT: out (vm); break;
            case POP: pop (vm); break;

            case CMP: cmp (vm); break;
            
            case ADD: STOPONERR ( add (vm) ) break;
            case SUB: STOPONERR ( sub (vm) ) break;
            case MUL: STOPONERR ( mul (vm) ) break;
            case DIV: STOPONERR ( div (vm) ) break;

            case PUSH: STOPONERR ( push (vm) ) break;
            case MOV:  STOPONERR ( mov  (vm) ) break;
            
            case JMP:   STOPONERR ( jmp (vm) ) break;
            case JNZ:   STOPONERR ( jnz (vm) ) break;
            case JZ:    STOPONERR ( jz ( vm) ) break;
            case JL:    STOPONERR ( jl ( vm) ) break;
            case JG:    STOPONERR ( jg ( vm) ) break;
            case JLE:   STOPONERR ( jle (vm) ) break;
            case JGE:   STOPONERR ( jge (vm) ) break;

            case HALT: halt = true; break;

            default:
                ErrAcc |= VM_ERRCODE (VM_OPCODENOTFOUND);
                log_srcerr (
                    bcname,
                    instrc,
                    "bytecode corruption",
                    "unknown instruction: \"%0X\"",
                    *vm->codeseg.rip
                );
                VMFree (vm);
                return ErrAcc;
        }
    }

    VMFree (vm);
    return ErrAcc;
}


Erracc_t run (const char* input)
{
    log_string ("<grn>starting...<dft>\n");

    if (runThread (input) == 0) log_string ("<grn>Work is done<dft>\n");

    return ErrAcc;
}