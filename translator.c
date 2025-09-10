#include "logger/logger.h"
#include "translator/translator.h"

#define LOGFILE "log.html"

int main (int argc, char** argv)
{
    log_start (LOGFILE);

    if (argc < 3)
    {
        log_err ("error", "no input file, usage: %s <i>input file</i> <i>output file</i>", argv[0]);
        exit (EXIT_FAILURE);
    }


    translate (argv[1], argv[2]);

    log_close();
    return 0;
}