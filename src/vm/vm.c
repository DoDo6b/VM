#include "vm.h"


static uint64_t push (FILE* src, StackHandler stack)
{
    uint64_t err = 0;

    assertStrict (src,   "received NULL");
    assertStrict (stackVerify (stack) == 0, "received NULL");

    operand_t operand = 0;
    if (fscanf (src, " #%d", &operand) < 1)
    {
        err |= MISSINGOPERAND;
        log_err ("runtime error", "missing operand");
        return err;
    }

    stackPush (stack, &operand);

    return err;
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

    char version[BUFSIZ];
    fseek (src, 0, SEEK_SET);
    fread (version, sizeof (RTASM_VER), 1, src);
    if (memcmp (RTASM_VER, version, sizeof (RTASM_VER)) != 0)
    {
        err |= WRONGVERSION;
        log_err ("error", "incompatible version of rtasm");
        return err;
    }

    size_t instructionC = 0;
    fseek (src, -(long)sizeof (size_t), SEEK_END);
    fread (&instructionC, sizeof (size_t), 1, src);
    log_string ("<grn>%llu opcode(s) have been read<dft>\n", instructionC);
    fseek (src, sizeof (RTASM_VER), SEEK_SET);

    StackHandler stack = stackInit (STACKSIZE, sizeof (operand_t));

    char buffer[BUFSIZ];

    for (size_t i = 0; i < instructionC; i++)
    {
        fgets (buffer, sizeof (buffer), src);
        
    }
    

    stackFree (stack);
    fclose (src);
    return err;
}