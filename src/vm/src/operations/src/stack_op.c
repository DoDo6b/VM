#include "../operations.h"

#define MODSRCMASK  ((mod >> 3) & 0x07)

void op_IN (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    operand_t operand = 0;
    while (!scanf ("%lld", &operand));

    stackPush (vm->stack, &operand);

    vm->codeseg.rip += sizeof (opcode_t);
    return;
}


void op_PUSH (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    vm->codeseg.rip += sizeof (opcode_t);

    opcode_t mod = *(const opcode_t*)vm->codeseg.rip;
    vm->codeseg.rip += sizeof (opcode_t);

    pointer_t ptr = UINT64_MAX;

    switch (mod >> 6)
    {
        case IMM: stackPush (vm->stack, vm->codeseg.rip); vm->codeseg.rip += sizeof (operand_t); break;

        case REG:
            if (MODSRCMASK <= NUM_REGS) stackPush (vm->stack, &vm->regs[MODSRCMASK]);
            else
            {
                ErrAcc |= VM_ERRCODE (VM_BYTECODECORRUPTED);
                log_err ("syntax error", "trying to push data from unknown register");
                return;
            }
            break;

        case MEM:
            if (MODSRCMASK == DISP64)
            {
                #pragma GCC diagnostic push
                #pragma GCC diagnostic ignored "-Wcast-align"

                ptr = *(const pointer_t*)vm->codeseg.rip;

                if (ptr >= vm->memseg.size)
                {
                    ErrAcc |= VM_ERRCODE (VM_SEGFAULT);
                    log_err ("runtime error", "segfault");
                }

                stackPush (vm->stack, vm->memseg.memory + ptr);

                #pragma GCC diagnostic pop

                vm->codeseg.rip += sizeof (pointer_t);
                break;
            }
            else
            {
                if (MODSRCMASK >= NUM_REGS)
                {
                    ErrAcc |= VM_ERRCODE (VM_BYTECODECORRUPTED);
                    log_err ("syntax error", "trying to push data from unknown register");
                    return;
                }

                #pragma GCC diagnostic push
                #pragma GCC diagnostic ignored "-Wcast-align"

                stackPush (vm->stack, vm->memseg.memory + vm->regs[MODSRCMASK]);

                #pragma GCC diagnostic pop

                break;
            }

        case OFF:
            if (MODSRCMASK == DISP64)
            {
                VMdump (vm);
                ErrAcc |= VM_ERRCODE (VM_BYTECODECORRUPTED);
                log_err ("error", "impossible r/m field in mod");
                return;
            }

            if (MODSRCMASK >= NUM_REGS)
            {
                ErrAcc |= VM_ERRCODE (VM_BYTECODECORRUPTED);
                log_err ("syntax error", "trying to push data from unknown register");
                return;
            }

            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wcast-align"

            ptr = (pointer_t)(*(const offset_t*)vm->codeseg.rip + vm->regs[MODSRCMASK]);

            if (ptr >= vm->memseg.size)
            {
                VMdump (vm);
                ErrAcc |= VM_ERRCODE (VM_SEGFAULT);
                log_err ("runtime error", "segfault");
            }

            stackPush (vm->stack, vm->memseg.memory + ptr);

            #pragma GCC diagnostic pop

            vm->codeseg.rip += sizeof (pointer_t);
            break;


        default:
            VMdump (vm);
            ErrAcc |= VM_ERRCODE (VM_BYTECODECORRUPTED);
            log_err ("error", "bytecode corrupted");
            return;
    }

    return;
}

void op_OUT (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    operand_t valOnTop = 0;
    stackTop (vm->stack, &valOnTop);
 
    printf ("%lld\n", valOnTop);

    vm->codeseg.rip += sizeof (opcode_t);
}

void op_POP (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    stackPop (vm->stack, NULL);

    vm->codeseg.rip += sizeof (opcode_t);
}