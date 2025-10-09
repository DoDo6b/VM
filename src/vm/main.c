#include "../included/logger/logger.h"
#include "run.h"

#define LOGFILE "log.html"


int main (int argc, char** argv)
{
    log_start (LOGFILE);
    log_string ("call: %s %s\n", argv[0], argv[1]);

    if (argc < 2)
    {
        log_err ("error", "no input file, usage: %s *input file*", argv[0]);
        exit (EXIT_FAILURE);
    }


    ErrAcc |= run (argv[1]);
    if (ErrAcc)
    {
        log_err ("runtime error", "execution has ended with code %llu", ErrAcc);
        exit (EXIT_FAILURE);
    }


    log_close();
    return 0;
}