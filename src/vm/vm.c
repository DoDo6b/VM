#include "vm.h"


static Erracc_t push (opcode_t opcode, FILE* src, const VM* vm)
{
    assertStrict (src,                          "received NULL");
    assertStrict (vm,                           "received NULL");
    assertStrict (stackVerify (vm->stack) == 0, "received NULL");

    operand_t operand = 0;
    opcode_t reg = opcode & UINT8_MAX;

    if (reg)
    {
        if (reg <= NUM_REGS) operand = vm->regs[reg];
        else
        {
            ErrAcc |= BYTECODECORRUPTED;
            log_err ("translation error", "bytecode corrupted");
            return ErrAcc;
        }
    }
    else fread (&operand, sizeof (operand_t), 1, src);

    
    stackPush (vm->stack, &operand);

    return ErrAcc;
}

static Erracc_t mov (opcode_t opcode, VM* vm)
{
    assertStrict (vm,                           "received NULL");
    assertStrict (stackVerify (vm->stack) == 0, "received NULL");

    opcode_t reg = opcode & UINT8_MAX;

    if (reg == 0)
    {
        ErrAcc |= BYTECODECORRUPTED;
        log_err ("translation error", "bytecode corrupted");
        exit (EXIT_FAILURE);
    }

    if (reg <= NUM_REGS) stackPop (vm->stack, &vm->regs[reg]);
    else
    {
        ErrAcc |= BYTECODECORRUPTED;
        log_err ("translation error", "bytecode corrupted");
        return ErrAcc;
    }

    return ErrAcc;
}

static void out (const VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    operand_t valOnTop = 0;
    stackTop (vm->stack, &valOnTop);
 
    printf ("%d\n", valOnTop);
}

static void pop (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    stackPop (vm->stack, NULL);
}

static uint64_t add (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    if (stackLen (vm->stack) < 2)
    {
        ErrAcc |= MISSINGOPERAND;
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

static uint64_t sub (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    if (stackLen (vm->stack) < 2)
    {
        ErrAcc |= MISSINGOPERAND;
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

static uint64_t mul (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    if (stackLen (vm->stack) < 2)
    {
        ErrAcc |= MISSINGOPERAND;
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

static uint64_t div (VM* vm)    //проверка на 0, надо бы добавить указатель на пользовательскую функцию обработчика
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    if (stackLen (vm->stack) < 2)
    {
        ErrAcc |= MISSINGOPERAND;
        log_err ("runtime error", "missing operand");
        return ErrAcc;
    }
    operand_t operand1 = 0;
    operand_t operand2 = 0;
    stackPop (vm->stack, &operand1);
    stackPop (vm->stack, &operand2);

    operand2 /= operand1;
    
    stackPush (vm->stack, &operand2);

    return ErrAcc;
}

Erracc_t run (const char* input)
{
    FILE* src = fopen (input, "rb");
    if (!src)
    {
        ErrAcc |= CANTOPEN;
        log_err ("fopen error", "cant open input file: %s", input);
        return ErrAcc; 
    }

    char   version[BUFSIZ] = {0};
    fread (version, sizeof (RTASM_VER), 1, src);

    if (memcmp (RTASM_VER, version, sizeof (RTASM_VER)) != 0)
    {
        ErrAcc |= WRONGVERSION;
        log_err ("error", "incompatible version of rtasm");
        return ErrAcc;
    }
    log_string ("<grn>version is compatible<dft>\n", input );

    size_t instructionCounter = 0;
    fseek (src, -(long)sizeof (size_t), SEEK_END);
    fread (&instructionCounter, sizeof (size_t), 1, src);
    log_string ("<grn>%llu opcode(s) will be executed<dft>\n", instructionCounter );
    fseek (src, sizeof (RTASM_VER), SEEK_SET);

    VM* vm = VMInit (STACKSIZE);

    opcode_t opcode = 0;

    for (size_t i = 0; i < instructionCounter; i++)
    {
        fread (&opcode, sizeof (opcode), 1, src);

        switch (opcode >> OPCODESHIFT)
        {
            case OUT: out (vm); break;
            case POP: pop (vm); break;
            case ADD: add (vm); break;
            case SUB: sub (vm); break;
            case MUL: mul (vm); break;
            case DIV: div (vm); break;

            case PUSH: push (opcode, src, vm); break;
            case MOV:  mov  (opcode,      vm); break;

            case HALT: i = instructionCounter; break;

            default:
                log_string (
                    "%s:%llu: <b><red>syntax error:<dft> unknown instruction</b>\n",
                    input,
                    instructionCounter + 1
                );
                ErrAcc |= SYNTAX;
                return ErrAcc;
        }
    }
    
    log_string ("<grn>Work is done<dft>\n");

    VMFree (vm);
    fclose (src);

    return ErrAcc;
}