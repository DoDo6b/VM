#include "reg_op.h"


Erracc_t mov (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");
    vm->codeseg.rip += sizeof (opcode_t);

    opcode_t mod = *(const opcode_t*)vm->codeseg.rip;
    vm->codeseg.rip += sizeof (opcode_t);

    switch (mod >> 6)
    {
        case REG:
            if ((mod & ~(3 << 6)) <= NUM_REGS) stackPop (vm->stack, &vm->regs[mod & ~(3 << 6)]);
            else
            {
                ErrAcc |= VM_ERRCODE (VM_BYTECODECORRUPTED);
                log_err ("translation error", "bytecode corrupted");
                return ErrAcc;
            }
            break;
        case MEM:
            if (*(const pointer_t*)vm->codeseg.rip >= vm->memseg.size)
            {
                ErrAcc |= VM_ERRCODE (VM_SEGFAULT);
                log_err ("runtime error", "segfault");
            }
            stackPop (vm->stack, vm->memseg.memory + *(const pointer_t*)vm->codeseg.rip);
            vm->codeseg.rip += sizeof (pointer_t);
            break;

        default:
            ErrAcc |= VM_ERRCODE (VM_BYTECODECORRUPTED);
            log_err ("translation error", "bytecode corrupted");
            return ErrAcc;
    }

    return ErrAcc;
}