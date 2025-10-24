#include "../included/logger/logger.h"
#include "translator.h"

#define LOGFILE "translator_log.html"


int main (int argc, char** argv)
{
    log_start (LOGFILE);
    log_string ("call: %s %s %s\n", argv[0], argv[1], argv[2]);

    if (argc < 3)
    {
        log_err ("call error", "no input file, usage: %s *input file* *output file*", argv[0]);
        return EXIT_FAILURE;
    }


    ErrAcc |= translate (argv[1], argv[2]);
    if (ErrAcc)
    {
        log_err ("translation error", "translation has ended with code: %llu", ErrAcc);
        return EXIT_FAILURE;
    }


    log_close();
    return 0;
}