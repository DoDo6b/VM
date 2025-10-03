#include "stack_op.h"

Erracc_t push (opcode_t opcode, Buffer* src, const VM* vm)
{
    assertStrict (src,                          "received NULL");
    assertStrict (VMVerify (vm) == 0,           "vm corrupted");
    assertStrict (stackVerify (vm->stack) == 0, "stack corrupted");

    operand_t operand = 0;
    opcode_t mod = opcode & UINT8_MAX;
    pointer_t ramPtr = 0;

    switch (mod >> 6)
    {
        case 0: bufCpy (src, &operand, sizeof (operand_t)); break;
        case 1:
            if (mod & 64 <= NUM_REGS) operand = vm->regs[mod & 64];
            else
            {
                ErrAcc |= BUF_ERRCODE (VM_BYTECODECORRUPTED);
                log_err ("translation error", "bytecode corrupted");
                return ErrAcc;
            }
            break;
        case 2:
            bufCpy (src, &ramPtr, sizeof (pointer_t));
            if (ramPtr >= vm->ram.size)
            {
                ErrAcc |= BUF_ERRCODE (VM_SEGFAULT);
                log_err ("runtime error", "segfault");
            }
            operand = vm->ram.data[ramPtr];
            break;
        default:
            ErrAcc |= BUF_ERRCODE (VM_BYTECODECORRUPTED);
            log_err ("translation error", "bytecode corrupted");
            return ErrAcc;
    }

    stackPush (vm->stack, &operand);

    return ErrAcc;
}

void out (const VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    operand_t valOnTop = 0;
    stackTop (vm->stack, &valOnTop);
 
    printf ("%d\n", valOnTop);
}

void pop (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    stackPop (vm->stack, NULL);
}