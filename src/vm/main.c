#include "../included/logger/logger.h"
#include "run.h"

#define LOGFILE "log.html"
#define STACKSIZE 2048
#define RAMSIZE 16384


int main (int argc, char** argv)
{
    log_start (LOGFILE);

    if (argc < 2)
    {
        log_err ("error", "no input file, usage: %s *input file*", argv[0]);
        return EXIT_FAILURE;
    }


    ErrAcc |= run (argv[1], STACKSIZE, RAMSIZE);
    if (ErrAcc)
    {
        log_err ("runtime error", "execution has ended with code %llu", ErrAcc);
        return EXIT_FAILURE;
    }


    log_close();
    return 0;
}