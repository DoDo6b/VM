#include "included/logger/logger.h"
#include "vm/vm.h"
#include "translator/translator.h"

#define LOGFILE "stdout"

int main (int argc, char** argv)
{
    log_start (LOGFILE);
    log_string ("call: %s %s %s\n", argv[0], argv[1], argv[2]);

    if (argc < 3)
    {
        log_err ("error", "no input file, usage: %s <i>input file</i> <i>output file</i>", argv[0]);
        exit (EXIT_FAILURE);
    }


    if (*argv[1] != '-')
    {
        ErrAcc |= translate (argv[1], argv[2]);
        if (ErrAcc)
        {
            log_err ("runtime error", "translation has ended with code %llu", ErrAcc);
            exit (EXIT_FAILURE);
        }
    }


    ErrAcc |= run (argv[2]);
    if (ErrAcc)
    {
        log_err ("runtime error", "execution has ended with code %llu", ErrAcc);
        exit (EXIT_FAILURE);
    }


    log_close();
    return 0;
}