#include "../operations.h"

Erracc_t add (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    vm->codeseg.rip += sizeof (opcode_t);

    if (stackLen (vm->stack) < 2)
    {
        VMdump (vm);
        ErrAcc |= VM_ERRCODE (VM_MISSINGOPERAND);
        log_err ("runtime error", "missing operand");
        return ErrAcc;
    }
    operand_t operand1 = 0;
    operand_t operand2 = 0;
    stackPop (vm->stack, &operand1);
    stackPop (vm->stack, &operand2);

    operand2 += operand1;
    
    stackPush (vm->stack, &operand2);

    return ErrAcc;
}

Erracc_t sub (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    vm->codeseg.rip += sizeof (opcode_t);

    if (stackLen (vm->stack) < 2)
    {
        VMdump (vm);
        ErrAcc |= VM_ERRCODE (VM_MISSINGOPERAND);
        log_err ("runtime error", "missing operand");
        return ErrAcc;
    }
    operand_t operand1 = 0;
    operand_t operand2 = 0;
    stackPop (vm->stack, &operand1);
    stackPop (vm->stack, &operand2);

    operand2 -= operand1;
    
    stackPush (vm->stack, &operand2);

    return ErrAcc;
}


Erracc_t mul (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    vm->codeseg.rip += sizeof (opcode_t);

    if (stackLen (vm->stack) < 2)
    {
        VMdump (vm);
        ErrAcc |= VM_ERRCODE (VM_MISSINGOPERAND);
        log_err ("runtime error", "missing operand");
        return ErrAcc;
    }
    operand_t operand1 = 0;
    operand_t operand2 = 0;
    stackPop (vm->stack, &operand1);
    stackPop (vm->stack, &operand2);

    operand2 *= operand1;
    
    stackPush (vm->stack, &operand2);

    return ErrAcc;
}

Erracc_t div (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    vm->codeseg.rip += sizeof (opcode_t);

    if (stackLen (vm->stack) < 2)
    {
        VMdump (vm);
        ErrAcc |= VM_ERRCODE (VM_MISSINGOPERAND);
        log_err ("runtime error", "missing operand");
        return ErrAcc;
    }
    operand_t operand1 = 0;

    if (operand1 == 0)
    {
        ErrAcc |= VM_ERRCODE (VM_DIVISIONBYZERO);
        log_err ("runtime error", "division by zero");
        return ErrAcc;
    }

    operand_t operand2 = 0;
    stackPop (vm->stack, &operand1);
    stackPop (vm->stack, &operand2);

    operand2 /= operand1;
    
    stackPush (vm->stack, &operand2);

    return ErrAcc;
}


Erracc_t cmp (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    vm->codeseg.rip += sizeof (opcode_t);

    if (stackLen (vm->stack) < 2)
    {
        VMdump (vm);
        ErrAcc |= VM_ERRCODE (VM_MISSINGOPERAND);
        log_err ("runtime error", "missing operand");
        return ErrAcc;
    }
    
    operand_t operandR = 0;
    operand_t operandL = 0;
    stackPop (vm->stack, &operandR);
    stackPop (vm->stack, &operandL);

    if (operandL == operandR) vm->rflags |=   1ULL << 6;
    else                      vm->rflags &= ~(1ULL << 6);
    if (operandL  < operandR) vm->rflags |=   1ULL;
    else                      vm->rflags &=  ~1ULL;

    return ErrAcc;
}