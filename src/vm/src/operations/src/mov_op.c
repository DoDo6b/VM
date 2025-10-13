#include "../operations.h"

#define MODDSTMASK (mod & 0x07)

Erracc_t mov (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");
    vm->codeseg.rip += sizeof (opcode_t);

    opcode_t mod = *(const opcode_t*)vm->codeseg.rip;
    vm->codeseg.rip += sizeof (opcode_t);

    pointer_t ptr = UINT64_MAX;

    switch (mod >> 6)
    {
        case REG:
            if (MODDSTMASK <= NUM_REGS) stackPop (vm->stack, &vm->regs[MODDSTMASK]);
            else
            {
                ErrAcc |= VM_ERRCODE (VM_BYTECODECORRUPTED);
                log_err ("syntax error", "trying to mov data into unknown register");
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
                    ErrAcc |= VM_ERRCODE (VM_SEGFAULT);
                    log_err ("runtime error", "segfault ptr: %llu > size: %zu", ptr, vm->memseg.size);
                }

                stackPop (vm->stack, vm->memseg.memory + ptr);

                #pragma GCC diagnostic pop

                vm->codeseg.rip += sizeof (pointer_t);
                break;
            }
            else
            {
                if (MODDSTMASK >= NUM_REGS)
                {
                    ErrAcc |= VM_ERRCODE (VM_BYTECODECORRUPTED);
                    log_err ("syntax error", "trying to mov data into unknown register");
                    return ErrAcc;
                }

                #pragma GCC diagnostic push
                #pragma GCC diagnostic ignored "-Wcast-align"

                stackPop (vm->stack, vm->memseg.memory + vm->regs[MODDSTMASK]);

                #pragma GCC diagnostic pop

                break;
            }

        case OFF:
            if (MODDSTMASK == DISP64)
            {
                ErrAcc |= VM_ERRCODE (VM_BYTECODECORRUPTED);
                log_err ("syntax error", "offset and disp64 combined in mov r/m mod");
                return ErrAcc;
            }

            if (MODDSTMASK >= NUM_REGS)
            {
                ErrAcc |= VM_ERRCODE (VM_BYTECODECORRUPTED);
                log_err ("syntax error", "trying to mov data into unknown register");
                return ErrAcc;
            }

            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wcast-align"

            ptr = (pointer_t)(*(const offset_t*)vm->codeseg.rip + vm->regs[MODDSTMASK]);

            if (ptr >= vm->memseg.size)
            {
                ErrAcc |= VM_ERRCODE (VM_SEGFAULT);
                log_err ("runtime error", "segfault");
            }

            stackPop (vm->stack, vm->memseg.memory + ptr);

            #pragma GCC diagnostic pop

            vm->codeseg.rip += sizeof (offset_t);
            break;


        default:
            ErrAcc |= VM_ERRCODE (VM_BYTECODECORRUPTED);
            log_err ("syntax error", "unknown mov r/m mod");
            return ErrAcc;
    }

    return ErrAcc;
}