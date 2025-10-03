#include "../vm.h"
#include "operations/arithmetic_op/arithmetic.h"
#include "operations/stack_op/stack_op.h"
#include "operations/jmp_op/jmp_op.h"
#include "operations/reg_op/reg_op.h"


static FILE* fileOpen (const char* fname, const char* attributes)
{
    assertStrict (fname,      "received NULL");
    assertStrict (attributes, "received NULL");

    FILE* stream = fopen (fname, attributes);

    if (!stream)
    {
        ErrAcc |= BUF_ERRCODE (VM_FOPENERR);
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
        ErrAcc |= BUF_ERRCODE (VM_BYTECODECORRUPTED);
        log_err ("error", "bytecode corrupted");
    }
    if (header->version != RTASM_VER)
    {
        ErrAcc |= BUF_ERRCODE (VM_WRONGVERSION);
        log_err ("error", "incompatible version of rtasm");
    }
    return ErrAcc;
}

static Erracc_t runThread (Buffer* srcBuf, size_t instrc)
{
    VM* vm = VMInit (STACKSIZE, RAMSIZE);

    opcode_t opcode = 0;

    bool halt = false;
    while (!halt)
    {
        bufCpy (srcBuf, &opcode, sizeof (opcode_t));
        switch (opcode >> OPCODESHIFT)
        {
            case OUT: out (vm); break;
            case POP: pop (vm); break;

            case CMP: cmp (vm); break;
            
            case ADD: add (vm); break;
            case SUB: sub (vm); break;
            case MUL: mul (vm); break;
            case DIV: div (vm); break;

            case PUSH: push (opcode, srcBuf, vm); break;
            case MOV:  mov  (opcode,         vm); break;
            
            case JMP:   jmp (srcBuf, vm);  break;
            case JNZ:   jnz (srcBuf, vm);  break;
            case JZ:    jz (srcBuf,  vm);  break;
            case JL:    jl (srcBuf,  vm);  break;
            case JG:    jg (srcBuf,  vm);  break;
            case JLE:   jle (srcBuf,  vm); break;
            case JGE:   jge (srcBuf,  vm); break;

            case HALT: halt = true; break;

            default:
                ErrAcc |= BUF_ERRCODE (VM_OPCODENOTFOUND);
                log_srcerr (
                    srcBuf->name,
                    instrc + 1,
                    "bytecode corruption",
                    "unknown instruction: \"%0X\"",
                    opcode >> OPCODESHIFT
                );
                VMFree (vm);
                return ErrAcc;
        }
    }
    return ErrAcc;
}


Erracc_t run (const char* input)
{
    FILE* srcStream = fileOpen (input, "rb");
    
    Buffer* srcBuf = bufInit ((size_t)fileSize (srcStream));
    bufSetStream (srcBuf, input, srcStream, BUFREAD);
    bufRead (srcBuf, 0);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
    const Header* header = (const Header*)srcBuf->bufpos;
#pragma GCC diagnostic pop
    bufSeek (srcBuf, sizeof (Header), SEEK_SET);

    if (headerCmp (header)) return ErrAcc;
    log_string ("<grn>version is compatible<dft>\n");

    log_string ("<grn>%llu opcode(s) will be executed<dft>\n", header->instrc);

    if (runThread (srcBuf, header->instrc) == 0) log_string ("<grn>Work is done<dft>\n");

    bufFree (srcBuf);
    fclose  (srcStream);

    return ErrAcc;
}