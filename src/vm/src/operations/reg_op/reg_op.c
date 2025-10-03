#include "reg_op.h"


Erracc_t mov (opcode_t opcode, Buffer* src, VM* vm)
{
    assertStrict (bufVerify (src, 0)      == 0, "received NULL");
    assertStrict (VMVerify (vm)           == 0, "vm corrupted");
    assertStrict (stackVerify (vm->stack) == 0, "stack corrupted");

    opcode_t mod = opcode & UINT8_MAX;
    pointer_t ramPtr = 0;

    switch (mod >> 6)
    {
        case 1:
            if (mod & 64 <= NUM_REGS) stackPop (vm->stack, &vm->regs[mod & 64]);
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
            stackPop (vm->stack, vm->ram.data + ramPtr);
            break;

        default:
            ErrAcc |= BUF_ERRCODE (VM_BYTECODECORRUPTED);
            log_err ("translation error", "bytecode corrupted");
            return ErrAcc;
    }

    return ErrAcc;
}