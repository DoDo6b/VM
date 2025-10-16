#include <unistd.h>
#include "../operations.h"

#define SLEEPTIME 100000

void draw (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

#ifdef _WIN32
    system ("cls");
#else
    system ("clear");
#endif

    const unsigned char* ptr = (const unsigned char*)vm->memseg.memory;

    for (; ptr < (const unsigned char*)vm->memseg.memory + vm->memseg.size; ptr++)
    {
        if (*ptr != 0) printf ("%c", *ptr);
        else printf (" ");
    }
    printf ("\n");

    usleep (SLEEPTIME);

    vm->codeseg.rip += sizeof (opcode_t);
}

void dmp (VM* vm)
{
    VMdump (vm);

    vm->codeseg.rip += sizeof (opcode_t);
}