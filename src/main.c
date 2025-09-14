#include "included/logger/logger.h"
#include "translator/translator.h"

#define LOGFILE "stdout"

int main (int argc, char** argv)
{
    log_start (LOGFILE);

    if (argc < 3)
    {
        log_err ("error", "no input file, usage: %s <i>input file</i> <i>output file</i>", argv[0]);
        exit (EXIT_FAILURE);
    }


    uint64_t errcode = translate (argv[1], argv[2]);

    if (errcode)
    {
        log_err ("runtime error", "translation has ended with code %llu", errcode);
        exit (EXIT_FAILURE);
    }

    log_close();
    return 0;
}