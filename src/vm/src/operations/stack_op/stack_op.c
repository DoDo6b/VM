#include "stack_op.h"

Erracc_t push (opcode_t opcode, Buffer* src, const VM* vm)
{
    assertStrict (src,                          "received NULL");
    assertStrict (VMVerify (vm) == 0,           "vm corrupted");
    assertStrict (stackVerify (vm->stack) == 0, "stack corrupted");

    operand_t operand = 0;
    opcode_t reg = opcode & UINT8_MAX;

    if (reg)
    {
        if (reg <= NUM_REGS) operand = vm->regs[reg];
        else
        {
            ErrAcc |= BUF_ERRCODE (VM_BYTECODECORRUPTED);
            log_err ("translation error", "bytecode corrupted");
            return ErrAcc;
        }
    }
    else bufCpy (src, &operand, sizeof (operand_t));

    
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