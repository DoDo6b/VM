#include "reg_op.h"


Erracc_t mov (opcode_t opcode, VM* vm)
{
    assertStrict (vm,                           "received NULL");
    assertStrict (stackVerify (vm->stack) == 0, "received NULL");

    opcode_t reg = opcode & UINT8_MAX;

    if (reg == 0)
    {
        ErrAcc |= BUF_ERRCODE (VM_BYTECODECORRUPTED);
        log_err ("translation error", "bytecode corrupted");
        exit (EXIT_FAILURE);
    }

    if (reg <= NUM_REGS) stackPop (vm->stack, &vm->regs[reg]);
    else
    {
        ErrAcc |= BUF_ERRCODE (VM_BYTECODECORRUPTED);
        log_err ("translation error", "bytecode corrupted");
        return ErrAcc;
    }

    return ErrAcc;
}