#include "src/framegen.h"

#define LOG "stdout"

int main (int argc, char** argv)
{
    log_start (LOG);

    if (argc < 4)
    {
        log_err ("call error", "usage: %s *input file* *w* *h*", argv[0]);
        exit (EXIT_FAILURE);
    }

    char* nothing;
    log_string ("Frames: %zu\n", animationGen (argv[1], (int)strtol(argv[2], &nothing, 10), (int)strtol(argv[3], &nothing, 10)));
    return 0;
}