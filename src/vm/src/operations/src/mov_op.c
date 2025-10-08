#include "../operations.h"

#define MODDSTMASK (mod & 0x07)

Erracc_t mov (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");
    vm->codeseg.rip += sizeof (opcode_t);

    opcode_t mod = *(const opcode_t*)vm->codeseg.rip;
    vm->codeseg.rip += sizeof (opcode_t);

    pointer_t ptr = -1;

    switch (mod >> 6)
    {
        case REG:
            if (MODDSTMASK <= NUM_REGS) stackPop (vm->stack, &vm->regs[MODDSTMASK]);
            else
            {
                VMdump (vm);
                ErrAcc |= VM_ERRCODE (VM_BYTECODECORRUPTED);
                log_err ("error", "bytecode corrupted");
                return ErrAcc;
            }
            break;

        case MEM:

            if (MODDSTMASK == DISP64)
            {
                #pragma GCC diagnostic push
                #pragma GCC diagnostic ignored "-Wcast-align"

                ptr = *(const pointer_t*)vm->codeseg.rip;

                if (ptr >= vm->memseg.size)
                {
                    VMdump (vm);
                    ErrAcc |= VM_ERRCODE (VM_SEGFAULT);
                    log_err ("runtime error", "segfault");
                }

                stackPop (vm->stack, vm->memseg.memory + ptr);

                #pragma GCC diagnostic pop

                vm->codeseg.rip += sizeof (pointer_t);
                break;
            }
            else
            {
                #pragma GCC diagnostic push
                #pragma GCC diagnostic ignored "-Wcast-align"

                stackPop (vm->stack, vm->memseg.memory + vm->regs[MODDSTMASK]);

                #pragma GCC diagnostic pop

                break;
            }

        case OFF:
            if (MODDSTMASK == DISP64)
            {
                VMdump (vm);
                ErrAcc |= VM_ERRCODE (VM_BYTECODECORRUPTED);
                log_err ("error", "impossible r/m field in mod");
                return ErrAcc;
            }

            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wcast-align"

            ptr = (pointer_t)(*(const offset_t*)vm->codeseg.rip + vm->regs[MODDSTMASK]);

            if (ptr >= vm->memseg.size)
            {
                VMdump (vm);
                ErrAcc |= VM_ERRCODE (VM_SEGFAULT);
                log_err ("runtime error", "segfault");
            }

            stackPop (vm->stack, vm->memseg.memory + ptr);

            #pragma GCC diagnostic pop

            vm->codeseg.rip += sizeof (pointer_t);
            break;


        default:
            VMdump (vm);
            ErrAcc |= VM_ERRCODE (VM_BYTECODECORRUPTED);
            log_err ("translation error", "bytecode corrupted");
            return ErrAcc;
    }

    return ErrAcc;
}