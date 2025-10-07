#include "segments.h"


static Erracc_t headerCmp (FILE* stream)
{
    assertStrict (stream, "received NULL");

    Header header = {};
    fseek (stream, 0, SEEK_SET);
    fread (&header, sizeof (Header), 1, stream);

    if (header.sign    != RTASM_SIGN)
    {
        ErrAcc |= VM_ERRCODE (VM_BYTECODECORRUPTED);
        log_err ("error", "header sign corrupted: %0llX", header.sign);
    }
    if (header.version != RTASM_VER)
    {
        ErrAcc |= VM_ERRCODE (VM_WRONGVERSION);
        log_err ("error", "incompatible version of rtasm: %0llX (current: %0llX)", header.version, RTASM_VER);
    }
    return ErrAcc;
}


Erracc_t buildCodeseg (CodeSeg* seg, const char* bcname)
{
    assertStrict (seg,    "received NULL");
    assertStrict (bcname, "received NULL");

    FILE* stream = fopen (bcname, "rb");
    if (!stream)
    {
        ErrAcc |= VM_ERRCODE (VM_FOPENERR);
        log_err ("fopen error", "cant open byte code");
        return ErrAcc;
    }

    if (headerCmp (stream) != 0)
    {
        log_err ("error", "header is corrupted");
        fclose (stream);
        return ErrAcc;
    }

    long size = fileSize (stream) - (ssize_t)sizeof (Header);
    if (size < 0)
    {
        ErrAcc |= VM_ERRCODE (VM_UNKNOWNERR);
        log_err ("filesys err", "file size < 0");
        return ErrAcc;
    }
    seg->size = (size_t)size;

    seg->code = (char*)calloc ((size_t)fileSize (stream), sizeof (char));
    if (!seg->code)
    {
        ErrAcc |= VM_ERRCODE (VM_ALLOCATIONERR);
        log_err ("internal error", "calloc returned NULL");
        fclose (stream);
        return ErrAcc;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
    if (fread ((void*)seg->code, sizeof (char), seg->size, stream) != seg->size)
#pragma GCC diagnostic pop
    {
        ErrAcc |= VM_ERRCODE (VM_FREADERR);
        log_err ("runtime warning", "read less than the file size is");
    }
    fclose (stream);

    seg->rip = seg->code; 

IF_DBG ( seg->hash = crc32Calculate ((const uint8_t*)seg->code, seg->size); )

    return ErrAcc;
}

Erracc_t codesegVerify (const CodeSeg* seg)
{
    if (!seg)
    {
        ErrAcc |= VM_ERRCODE (VM_NULLRECEIVED);
        log_err ("verification error", "received NULL");
        return ErrAcc;
    }

    if (!seg->code)
    {
        ErrAcc |= VM_ERRCODE (VM_NULLRECEIVED);
        log_err ("verification error", "code segment is NULL");
        return ErrAcc;
    }

    if (seg->code > seg->rip || seg->rip > seg->code + seg->size)
    {
        ErrAcc |= VM_ERRCODE (VM_SEGFAULT);
        log_err ("verification error", "rip is out of segment (%p : <red> %p <dft> : %p)", seg->code, seg->rip, seg->code + seg->size);
    }

IF_DBG
(
    if (seg->hash != crc32Calculate ((const uint8_t*)seg->code, seg->size))
    {
        ErrAcc |= VM_ERRCODE (VM_SEGFAULT);
        log_err ("verification error", "code segment hash is different");
    }
)

    return ErrAcc;
}

Erracc_t codesegDump (const CodeSeg* seg)
{
    assertStrict (codesegVerify (seg) == 0, "code segment corrupted");
    log_string ("<blu>code section(segment) dump:<dft>\n");
    log_string
    (
        "segment start:     0x%p\n"
        "segment size:      %zu\n"
        "segment end:       0x%p\n"
IF_DBG( "segment checksum:  0x%p\n"),
        seg->code,
        seg->size,
        seg->code + seg->size
IF_DBG(,seg->hash)
    );

    log_string ("rip:               0x%p\n{\n", seg->rip);
    const char* dumpStart = seg->rip > seg->code + 5 ? seg->rip - 5 : seg->code;
    size_t dumpSize = seg->size > 10 ? 10 : seg->size;
    
    log_string ("<blk>  ");
    size_t i = 0;
    for (; i < (size_t)(seg->rip - dumpStart) && i < dumpSize; i++) log_string ("%02zX ", i);
    log_string ("<dft><mgn>%02zX<dft><blk> ", i++);
    for (; i < dumpSize; i++) log_string ("%02zX ", i);

    log_string ("<dft>\n  <cyn>");
    for (i = 0; i < (size_t)(seg->rip - dumpStart) && i < dumpSize; i++) log_string ("%02hhX ", *(dumpStart+i));
    log_string ("<dft><mgn>%02hhX<dft><cyn> ", *(dumpStart+i++));
    for (; i < dumpSize; i++) log_string ("%02hhX ", *(dumpStart+i));
    log_string ("<dft>\n}\n");

    log_string ("<blk>dump:<dft>\n");
    memBlockDump (seg->code, seg->size, 16);

    return ErrAcc;
}

void freeCodeseg (CodeSeg* dst)
{
    if (dst)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
        if (dst->code) free ((void*)dst->code);
#pragma GCC diagnostic pop
        memset (dst, 0, sizeof (CodeSeg));
    }
}



Erracc_t buildRAMseg (RAMseg* dst, size_t size)
{
    assertStrict (dst, "received NULL");

    dst->memory = (char*)calloc (size, sizeof (char));
    if (!dst->memory)
    {
        ErrAcc |= VM_ERRCODE (VM_ALLOCATIONERR);
        log_err ("internal error", "calloc returned NULL");
        return ErrAcc;
    }

    dst->size = size;

    return ErrAcc;
}

Erracc_t RAMsegverify (const RAMseg* seg)
{
    if (!seg)
    {
        ErrAcc |= VM_ERRCODE (VM_NULLRECEIVED);
        log_err ("verification error", "received NULL");
        return ErrAcc;
    }

    if (!seg->memory)
    {
        ErrAcc |= VM_ERRCODE (VM_NULLRECEIVED);
        log_err ("verification error", "memory wasnt initialized");
    }

    return ErrAcc;
}

Erracc_t RAMdump (const RAMseg* seg)
{
    assertStrict (RAMsegverify (seg) == 0, "RAM corrupted");

    log_string ("<blu>RAM dump:<dft>\n");
    log_string 
    (
        "segment start: 0x%p\n"
        "segment size:  %zu\n"
        "segment end:   0x%p\n",
        seg->memory,
        seg->size,
        seg->memory + seg->size
    );

    log_string ("<blk>RAM dump:<dft>\n");
    memBlockDump (seg->memory, seg->size, 16);

    return ErrAcc;
}

void freeRAMseg (RAMseg* dst)
{
    if (dst)
    {
        if (dst->memory) free (dst->memory);
        memset (dst, 0, sizeof (RAMseg));
    }
}