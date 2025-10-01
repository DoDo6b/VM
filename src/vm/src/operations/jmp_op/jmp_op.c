#include "jmp_op.h"


Erracc_t jmp (Buffer* src, VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");
    assertStrict (bufVerify (src, 0) == 0 && src->mode == BUFREAD, "buf corrupted");

    pointer_t jmp = 0;
    bufCpy (src, &jmp, sizeof (pointer_t));
    bufSeek (src, jmp + sizeof (Header), SEEK_SET);

    return ErrAcc;
}

Erracc_t jnz (Buffer* src, VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");
    assertStrict (bufVerify (src, 0) == 0 && src->mode == BUFREAD, "buf corrupted");

    if (!vm->zf) jmp (src, vm);
    else bufSeek (src, sizeof (pointer_t), SEEK_CUR);
    return ErrAcc;
}

Erracc_t jz (Buffer* src, VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");
    assertStrict (bufVerify (src, 0) == 0 && src->mode == BUFREAD, "buf corrupted");
    
    if (vm->zf) jmp (src, vm);
    else bufSeek (src, sizeof (pointer_t), SEEK_CUR);
    return ErrAcc;
}

Erracc_t jl (Buffer* src, VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");
    assertStrict (bufVerify (src, 0) == 0 && src->mode == BUFREAD, "buf corrupted");
    
    if (!vm->zf && vm->cf) jmp (src, vm);
    else bufSeek (src, sizeof (pointer_t), SEEK_CUR);
    return ErrAcc;
}

Erracc_t jg (Buffer* src, VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");
    assertStrict (bufVerify (src, 0) == 0 && src->mode == BUFREAD, "buf corrupted");
    
    if (!vm->zf && !vm->cf) jmp (src, vm);
    else bufSeek (src, sizeof (pointer_t), SEEK_CUR);
    return ErrAcc;
}

Erracc_t jle (Buffer* src, VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");
    assertStrict (bufVerify (src, 0) == 0 && src->mode == BUFREAD, "buf corrupted");
    
    if (vm->zf || vm->cf) jmp (src, vm);
    else bufSeek (src, sizeof (pointer_t), SEEK_CUR);
    return ErrAcc;
}

Erracc_t jge (Buffer* src, VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");
    assertStrict (bufVerify (src, 0) == 0 && src->mode == BUFREAD, "buf corrupted");
    
    if (vm->zf || !vm->cf) jmp (src, vm);
    else bufSeek (src, sizeof (pointer_t), SEEK_CUR);
    return ErrAcc;
}