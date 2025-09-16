#include "vm.h"


static uint64_t push (opcode_t opcode, FILE* src, const VM* vm)
{
    assertStrict (src,                          "received NULL");
    assertStrict (vm,                           "received NULL");
    assertStrict (stackVerify (vm->stack) == 0, "received NULL");

    operand_t operand = 0;
    opcode_t reg = opcode & UINT8_MAX;

    if (reg)
    {
        switch (reg)
        {
            case AAX: operand = vm->aax; break;
            case ACX: operand = vm->acx; break;
            case ADX: operand = vm->adx; break;
            case ABX: operand = vm->abx; break;
            case ASP: operand = vm->asp; break;
            case ABP: operand = vm->abp; break;
            case ASI: operand = vm->asi; break;
            case ADI: operand = vm->adi; break;

            default:
                ErrAcc |= BYTECODECORRUPTED;
                log_err ("translation error", "bytecode corrupted");
                exit (EXIT_FAILURE);
        }
    }
    else fread (&operand, sizeof (operand_t), 1, src);

    stackPush (vm->stack, &operand);

    return ErrAcc;
}

static uint64_t mov (opcode_t opcode, VM* vm)
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

    switch (reg)
    {
        case AAX: stackPop (vm->stack, &vm->aax); break;
        case ACX: stackPop (vm->stack, &vm->acx); break;
        case ADX: stackPop (vm->stack, &vm->adx); break;
        case ABX: stackPop (vm->stack, &vm->abx); break;
        case ASP: stackPop (vm->stack, &vm->asp); break;
        case ABP: stackPop (vm->stack, &vm->abp); break;
        case ASI: stackPop (vm->stack, &vm->asi); break;
        case ADI: stackPop (vm->stack, &vm->adi); break;

        default:
            ErrAcc |= BYTECODECORRUPTED;
            log_err ("translation error", "bytecode corrupted");
            exit (EXIT_FAILURE);
    }

    return ErrAcc;
}

static void out (StackHandler stack)
{
    assertStrict (stackVerify (stack) == 0, "received NULL");

    operand_t valOnTop = 0;
    stackTop (stack, &valOnTop);
 
    printf ("%d\n", valOnTop);
}

static void pop (StackHandler stack)
{
    assertStrict (stackVerify (stack) == 0, "received NULL");

    stackPop (stack, NULL);
}

static uint64_t add (StackHandler stack)
{
    uint64_t err = 0;

    assertStrict (stackVerify (stack) == 0, "received NULL");

    if (stackLen (stack) < 2)
    {
        err |= MISSINGOPERAND;
        log_err ("runtime error", "missing operand");
        return err;
    }
    operand_t operand1 = 0;
    operand_t operand2 = 0;
    stackPop (stack, &operand1);
    stackPop (stack, &operand2);

    operand2 += operand1;
    
    stackPush (stack, &operand2);

    return err;
}

static uint64_t sub (StackHandler stack)
{
    uint64_t err = 0;

    assertStrict (stackVerify (stack) == 0, "received NULL");

    if (stackLen (stack) < 2)
    {
        err |= MISSINGOPERAND;
        log_err ("runtime error", "missing operand");
        return err;
    }
    operand_t operand1 = 0;
    operand_t operand2 = 0;
    stackPop (stack, &operand1);
    stackPop (stack, &operand2);

    operand2 -= operand1;
    
    stackPush (stack, &operand2);

    return err;
}

static uint64_t mul (StackHandler stack)
{
    uint64_t err = 0;

    assertStrict (stackVerify (stack) == 0, "received NULL");

    if (stackLen (stack) < 2)
    {
        err |= MISSINGOPERAND;
        log_err ("runtime error", "missing operand");
        return err;
    }
    operand_t operand1 = 0;
    operand_t operand2 = 0;
    stackPop (stack, &operand1);
    stackPop (stack, &operand2);

    operand2 *= operand1;
    
    stackPush (stack, &operand2);

    return err;
}

static uint64_t div (StackHandler stack)
{
    uint64_t err = 0;

    assertStrict (stackVerify (stack) == 0, "received NULL");

    if (stackLen (stack) < 2)
    {
        err |= MISSINGOPERAND;
        log_err ("runtime error", "missing operand");
        return err;
    }
    operand_t operand1 = 0;
    operand_t operand2 = 0;
    stackPop (stack, &operand1);
    stackPop (stack, &operand2);

    operand2 /= operand1;
    
    stackPush (stack, &operand2);

    return err;
}

uint64_t run (const char* input)
{
    uint64_t err = 0;

    FILE* src = fopen (input, "rb");
    if (!src)
    {
        err |= CANTOPEN;
        log_err ("fopen error", "cant open input file: %s", input);
        return err; 
    }

    char   version[BUFSIZ];
    fread (version, sizeof (RTASM_VER), 1, src);

    if (memcmp (RTASM_VER, version, sizeof (RTASM_VER)) != 0)
    {
        err |= WRONGVERSION;
        log_err ("error", "incompatible version of rtasm");
        return err;
    }
    log_string ("<grn>version is compatible<dft>\n", input );

    size_t instructionCounter = 0;
    fseek (src, -(long)sizeof (size_t), SEEK_END);
    fread (&instructionCounter, sizeof (size_t), 1, src);
    log_string ("<grn>%llu opcode(s) will be executed<dft>\n", instructionCounter );
    fseek (src, sizeof (RTASM_VER), SEEK_SET);

    VM vm = {0};
    vm.stack = stackInit (STACKSIZE, sizeof (operand_t));

    opcode_t opcode = 0;

    for (size_t i = 0; i < instructionCounter; i++)
    {
        fread (&opcode, sizeof (opcode), 1, src);

        switch (opcode >> OPCODESHIFT)
        {
            case OUT: out (vm.stack); break;
            case POP: pop (vm.stack); break;
            case ADD: add (vm.stack); break;
            case SUB: sub (vm.stack); break;
            case MUL: mul (vm.stack); break;
            case DIV: div (vm.stack); break;

            case PUSH: push (opcode, src, &vm); break;
            case MOV:  mov  (opcode,      &vm); break;

            case HALT: i = instructionCounter; break;

            default:
                log_string (
                    "%s:%llu: <b><red>syntax error:<dft> unknown instruction</b>\n",
                    input,
                    instructionCounter + 1
                );
                err |= SYNTAX;
                return err;
        }
    }
    
    log_string ("<grn>Work is done<dft>\n");

    stackFree (vm.stack);
    fclose (src);

    return err;
}