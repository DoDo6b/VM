#include "vm.h"


static uint64_t push (FILE* src, StackHandler stack)
{
    log_string ("<grn>%s was called<dft>\n", __func__);

    uint64_t err = 0;

    assertStrict (src,   "received NULL");
    assertStrict (stackVerify (stack) == 0, "received NULL");

    operand_t operand = 0;
    fread (&operand, sizeof (operand_t), 1, src);
    log_string ("  <grn>read: %d<dft>\n", operand);

    stackPush (stack, &operand);
    log_string ("  <grn>pushed: %d<dft>\n", operand);

    log_string ("<grn>%s has finished<dft>\n", __func__);
    return err;
}

static void out (StackHandler stack)
{
    log_string ("<grn>%s was called<dft>\n", __func__);

    assertStrict (stackVerify (stack) == 0, "received NULL");

    operand_t valOnTop = 0;
    stackTop (stack, &valOnTop);
    log_string ("  <grn>value on top: %d<dft>\n", valOnTop);

    printf ("%d\n", valOnTop);

    log_string ("<grn>%s has finished<dft>\n", __func__);
}

static void pop (StackHandler stack)
{
    log_string ("<grn>%s was called<dft>\n", __func__);

    assertStrict (stackVerify (stack) == 0, "received NULL");

    stackPop (stack, NULL);
    log_string ("  <grn>poped<dft>\n");

    log_string ("<grn>%s has finished<dft>\n", __func__);
}

static uint64_t add (StackHandler stack)
{
    log_string ("<grn>%s was called<dft>\n", __func__);

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
    log_string ("  <grn>poped: %d<dft>\n", operand1);
    stackPop (stack, &operand2);
    log_string ("  <grn>poped: %d<dft>\n", operand2);

    operand2 += operand1;
    
    stackPush (stack, &operand2);
    log_string ("  <grn>pushed: %d<dft>\n", operand2);

    log_string ("<grn>%s has finished<dft>\n", __func__);
    return err;
}

static uint64_t sub (StackHandler stack)
{
    log_string ("<grn>%s was called<dft>\n", __func__);
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
    log_string ("  <grn>poped: %d<dft>\n", operand1);
    stackPop (stack, &operand2);
    log_string ("  <grn>poped: %d<dft>\n", operand2);

    operand2 -= operand1;
    
    stackPush (stack, &operand2);
    log_string ("  <grn>pushed: %d<dft>\n", operand2);

    log_string ("<grn>%s has finished<dft>\n", __func__);
    return err;
}

static uint64_t mul (StackHandler stack)
{
    log_string ("<grn>%s was called<dft>\n", __func__);

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
    log_string ("  <grn>poped: %d<dft>\n", operand1);
    stackPop (stack, &operand2);
    log_string ("  <grn>poped: %d<dft>\n", operand2);

    operand2 *= operand1;
    
    stackPush (stack, &operand2);
    log_string ("  <grn>pushed: %d<dft>\n", operand2);

    log_string ("<grn>%s has finished<dft>\n", __func__);
    return err;
}

static uint64_t div (StackHandler stack)
{
    log_string ("<grn>%s was called<dft>\n", __func__);

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
    log_string ("  <grn>poped: %d<dft>\n", operand1);
    stackPop (stack, &operand2);
    log_string ("  <grn>poped: %d<dft>\n", operand2);

    operand2 /= operand1;
    
    stackPush (stack, &operand2);
    log_string ("  <grn>pushed: %d<dft>\n", operand2);

    log_string ("<grn>%s has finished<dft>\n", __func__);
    return err;
}

uint64_t run (const char* input)
{
    log_string ("<grn>%s was called<dft>\n", __func__);

    uint64_t err = 0;

    FILE* src = fopen (input, "rb");
    if (!src)
    {
        err |= CANTOPEN;
        log_err ("fopen error", "cant open input file: %s", input);
        return err; 
    }
    log_string ("  <grn>succesfully opened(created) %s<dft>\n", input);

    char   version[BUFSIZ];
    fread (version, sizeof (RTASM_VER), 1, src);
    log_string ("  <ylw>version of rtasm: %s<dft>\n", version);

    if (memcmp (RTASM_VER, version, sizeof (RTASM_VER)) != 0)
    {
        err |= WRONGVERSION;
        log_err ("error", "incompatible version of rtasm");
        return err;
    }
    log_string ("  <grn>version is compatible<dft>\n", input);

    size_t instructionCounter = 0;
    fseek (src, -(long)sizeof (size_t), SEEK_END);
    fread (&instructionCounter, sizeof (size_t), 1, src);
    log_string ("<grn>%llu opcode(s) will be executed<dft>\n", instructionCounter);
    fseek (src, sizeof (RTASM_VER), SEEK_SET);

    StackHandler stack = stackInit (STACKSIZE, sizeof (operand_t));

    opcode_t opcode = 0;

    for (size_t i = 0; i < instructionCounter; i++)
    {
        fread (&opcode, sizeof (opcode), 1, src);
        log_string ("  <ylw>read opcode: %0X<dft>\n", opcode);

        switch (opcode)
        {
            case OUT: out (stack); break;
            case POP: pop (stack); break;
            case ADD: add (stack); break;
            case SUB: sub (stack); break;
            case MUL: mul (stack); break;
            case DIV: div (stack); break;

            case PUSH: push (src, stack); break;

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

    stackFree (stack);
    fclose (src);

    log_string ("<grn>%s has finished<dft>\n", __func__);
    return err;
}