#include "../operations.h"

#define WIDTH  100

void draw (VM* vm)
{
    assertStrict (VMVerify (vm) == 0, "vm corrupted");

    vm->codeseg.rip += sizeof (opcode_t);

#ifdef _WIN32
    system ("cls");
#else
    system ("clear");
#endif

    const unsigned char* ptr = (const unsigned char*)vm->memseg.memory;

    for (size_t y = 0; y < vm->memseg.size; y += WIDTH, ptr += WIDTH)
    {
        for (size_t x = 0; x < WIDTH && y + x < vm->memseg.size; x++)
        {
           if (*(ptr + x) != 0) printf ("%c", *(ptr + x));
           else printf (" ");
        }
        printf ("\n");
    }
}

void dmp (VM* vm)
{
    VMdump (vm);

    vm->codeseg.rip += sizeof (opcode_t);
}