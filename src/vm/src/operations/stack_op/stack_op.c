#include "stack_op.h"

Erracc_t push (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    vm->codeseg.rip += sizeof (opcode_t);

    opcode_t mod = *(const opcode_t*)vm->codeseg.rip;
    vm->codeseg.rip += sizeof (opcode_t);

    switch (mod >> 6)
    {
        case 0: stackPush (vm->stack, vm->codeseg.rip); vm->codeseg.rip += sizeof (operand_t); break;
        case 1:
            if ((mod & ~(3 << 6)) <= NUM_REGS) stackPush (vm->stack, &vm->regs[mod & ~(3 << 6)]);
            else
            {
                ErrAcc |= VM_ERRCODE (VM_BYTECODECORRUPTED);
                log_err ("translation error", "unknown reg (mod: %c)", mod);
                return ErrAcc;
            }
            break;
        case 2:
            if (*(const pointer_t*)vm->codeseg.rip >= vm->memseg.size)
            {
                ErrAcc |= VM_ERRCODE (VM_SEGFAULT);
                log_err ("runtime error", "segfault");
            }
            stackPush (vm->stack, vm->memseg.memory + *(const pointer_t*)vm->codeseg.rip);
            break;
        default:
            ErrAcc |= VM_ERRCODE (VM_BYTECODECORRUPTED);
            log_err ("translation error", "bytecode corrupted");
            return ErrAcc;
    }

    return ErrAcc;
}

void out (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    vm->codeseg.rip += sizeof (opcode_t);

    operand_t valOnTop = 0;
    stackTop (vm->stack, &valOnTop);
 
    printf ("%lld\n", valOnTop);
}

void pop (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    vm->codeseg.rip += sizeof (opcode_t);

    stackPop (vm->stack, NULL);
}