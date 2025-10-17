#include "src/animagen.h"

#define LOG "stdout"

int main (int argc, char** argv)
{
    log_start (LOG);

    if (argc < 2)
    {
        log_err ("call error", "usage: %s *input file*", argv[0]);
        exit (EXIT_FAILURE);
    }

    log_string ("Frames: %zu\n", animationGen (argv[1]));
    return 0;
}