#include "vm.h"


static Erracc_t push (opcode_t opcode, Buffer* src, const VM* vm)
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
    else bufCpy (src, &operand, sizeof (operand_t));

    
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


static FILE* fileOpen (const char* fname, const char* attributes)
{
    assertStrict (fname,      "received NULL");
    assertStrict (attributes, "received NULL");

    FILE* stream = fopen (fname, attributes);

    if (!stream)
    {
        ErrAcc |= FOPENERR;
        log_err ("fopen error", "cant open input file: \"%s\"", stream);
        return NULL;
    }

    return stream;
}

static Erracc_t headerCmp (const Header* header)
{
    assertStrict (header, "received NULL");

    if (header->sign    != RTASM_SIGN)
    {
        ErrAcc |= BYTECODECORRUPTED;
        log_err ("error", "bytecode corrupted");
    }
    if (header->version != RTASM_VER)
    {
        ErrAcc |= WRONGVERSION;
        log_err ("error", "incompatible version of rtasm");
    }
    return ErrAcc;
}


Erracc_t run (const char* input)
{
    FILE* srcStream = fileOpen (input, "rb");
    
    Buffer* srcBuf = bufInit (fileSize (srcStream));
    bufSetStream (srcBuf, srcStream, BUFREAD);
    bufRead (srcBuf, 0);

    Header* header = (Header*)srcBuf->bufpos;
    bufSeek (srcBuf, sizeof (Header), SEEK_SET);

    if (headerCmp (header)) return ErrAcc;
    log_string ("<grn>version is compatible<dft>\n");

    log_string ("<grn>%llu opcode(s) will be executed<dft>\n", header->instrc);

    VM* vm = VMInit (STACKSIZE);

    opcode_t opcode = 0;

    for (size_t i = 0; i < header->instrc; i++)
    {
        bufCpy (srcBuf, &opcode, sizeof (opcode_t));

        switch (opcode >> OPCODESHIFT)
        {
            case OUT: out (vm); break;
            case POP: pop (vm); break;
            case ADD: add (vm); break;
            case SUB: sub (vm); break;
            case MUL: mul (vm); break;
            case DIV: div (vm); break;

            case PUSH: push (opcode, srcBuf, vm); break;
            case MOV:  mov  (opcode,         vm); break;

            case HALT: i = header->instrc; break;

            default:
                log_string (
                    "%s:%llu: <b><red>syntax error:<dft> unknown instruction: \"%0X\"</b>\n",
                    input,
                    header->instrc + 1,
                    opcode >> OPCODESHIFT
                );
                ErrAcc |= SYNTAX;
                return ErrAcc;
        }
    }
    
    log_string ("<grn>Work is done<dft>\n");

    VMFree  (vm);
    bufFree (srcBuf);
    fclose  (srcStream);

    return ErrAcc;
}