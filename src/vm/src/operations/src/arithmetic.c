#include "../operations.h"

#define ARITHMETIC_OP(sign, name, condition)\
    void op ## name (VM* vm)\
    {\
        assertStrict (VMVerify (vm) == 0, "vm corrupted");\
        \
        if (stackLen (vm->stack) < 2)\
        {\
            VMdump (vm);\
            ErrAcc |= VM_ERRCODE (VM_MISSINGOPERAND);\
            log_err ("runtime error", "missing operand in stack");\
            return;\
        }\
        operand_t operand1 = 0;\
        operand_t operand2 = 0;\
        stackPop (vm->stack, &operand1);\
        stackPop (vm->stack, &operand2);\
        \
        condition\
        \
        operand2 sign ## = operand1;\
        \
        stackPush (vm->stack, &operand2);\
        \
        vm->codeseg.rip += sizeof (opcode_t);\
        return;\
    }

ARITHMETIC_OP (+, ADD, {})

ARITHMETIC_OP (-, SUB, {})

ARITHMETIC_OP (*, MUL, {})

ARITHMETIC_OP (/, DIV, 
    if (operand1 == 0)
    {
        VMdump (vm);
        ErrAcc |= VM_ERRCODE (VM_DIVISIONBYZERO);
        log_err ("runtime error", "division by zero");
        return;
    }
)

#undef ARITHMETIC_OP


void opCMP (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    if (stackLen (vm->stack) < 2)
    {
        VMdump (vm);
        ErrAcc |= VM_ERRCODE (VM_MISSINGOPERAND);
        log_err ("runtime error", "missing operand in stack");
        return;
    }
    
    operand_t operandR = 0;
    operand_t operandL = 0;
    stackPop (vm->stack, &operandR);
    stackPop (vm->stack, &operandL);

    if (operandL == operandR) vm->rflags |=   1ULL << 6;
    else                      vm->rflags &= ~(1ULL << 6);
    if (operandL  < operandR) vm->rflags |=   1ULL;
    else                      vm->rflags &=  ~1ULL;

    vm->codeseg.rip += sizeof (opcode_t);
    return;
}