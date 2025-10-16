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

        switch (*vm->codeseg.rip)
        {
            case OUT:  out  (vm); break;
            case POP:  pop  (vm); break;

            case CMP:  cmp  (vm); break;
            
            case ADD:  add  (vm); break;
            case SUB:  sub  (vm); break;
            case MUL:  mul  (vm); break;
            case DIV:  div  (vm); break;

            case IN:    in  (vm); break;

            case PUSH:  push(vm); break;
            case MOV:   mov (vm); break;
            
            case JMP:   jmp (vm); break;
            case JNZ:   jnz (vm); break;
            case JZ:    jz  (vm); break;
            case JL:    jl  (vm); break;
            case JG:    jg  (vm); break;
            case JLE:   jle (vm); break;
            case JGE:   jge (vm); break;

            case CALL:  call(vm); break;
            case RET:   ret (vm); break;

            case DRAW:  draw(vm); break;
            case DMP:   dmp (vm); break;

            case HALT: halt = true; break;

            default:
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