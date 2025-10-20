#include "animagen.h"

typedef struct
{
    FILE*   source;
    Buffer* srcBuf;
    FILE*   destination;
    Buffer* dstBuf;
}Descriptors;


static int init (const char* gif, Descriptors* descr)
{
    assertStrict (gif, "received NULL");

    descr->source = fopen (gif, "r");
    if (descr->source == NULL)
    {
        log_err ("fs error", "cant open animation");
        return -1;
    }

    char testL[BUFSIZ] = {};
    fgets (testL, sizeof (testL), descr->source);
    size_t w = strlen (testL);
    size_t h = 0;
    while (*testL != '\n')
    {
        fgets (testL, sizeof (testL), descr->source);
        h++;
    }
    fseek (descr->source, 0, SEEK_SET);
    log_string ("framew: %llu frameh: %llu\n", w, h);

    descr->srcBuf = bufInit (w * h);
    if (descr->srcBuf == NULL)
    {
        log_err ("alloc error", "cant create input buffer");
        return -1;
    }
    bufSetStream (descr->srcBuf, gif, descr->source, BUFREAD);


    char* outfname = (char*)calloc (strlen (gif) + sizeof (".artout"), sizeof (char));
    if (outfname == NULL)
    {
        log_err ("alloc errror", "cant allocate str buffer");
        return -1;
    }
    memcpy (outfname, gif, strlen (gif));
    strcat (outfname, ".artout");


    descr->destination = fopen (outfname, "w+");
    if (descr->destination == NULL)
    {
        log_err ("fs error", "cant open output file");
        return -1;
    }

    descr->dstBuf = bufInit (BUFSIZ);
    if (descr->dstBuf == NULL)
    {
        log_err ("alloc error", "cant create output buffer");
        return -1;
    }
    bufSetStream (descr->dstBuf, outfname, descr->destination, BUFWRITE);


    return 0;
}

static void release (Descriptors* descr)
{
    if (descr->srcBuf)
    {
        bufFree (descr->srcBuf);
    }
    if (descr->source)
    {
        fclose (descr->source);
    }

    if (descr->dstBuf)
    {
        bufFree (descr->dstBuf);
    }
    if (descr->destination)
    {
        fclose (descr->destination);
    }
}


static void framegen (Buffer* bufR, Buffer* bufW)
{
    assertStrict (bufVerify (bufR, 0) == 0 && bufR->mode == BUFREAD,  "bufR failed verification");
    assertStrict (bufVerify (bufW, 0) == 0 && bufW->mode == BUFWRITE, "bufW failed verification");

    bufFlush (bufR);
    size_t frame = bufRead  (bufR, 0);
    fseek (bufR->stream, 1, SEEK_CUR);

    if (frame != bufR->size)
    {
        log_err ("warning", "cropped frame detected");
    }

    char buf[16] = {0};
    for (size_t i = 0; i < frame; i++)
    {
        sprintf (buf, "%d", (int)bufGetc (bufR));

        bufWrite (bufW, PUSH,   sizeof (PUSH) - 1);
        bufWrite (bufW, buf,    strlen (buf));
        bufWrite (bufW, "\n",   sizeof ("\n") - 1);

        bufWrite (bufW, MOV,    sizeof (MOV) - 1);
        bufWrite (bufW, "[",    sizeof ("[") - 1);

        sprintf (buf, "%zu", i);
        bufWrite (bufW, buf,    strlen (buf));
        
        bufWrite (bufW, "]\n",  sizeof ("]\n") - 1);
    }

    bufWrite (bufW, DRAW, sizeof (DRAW) - 1);
}


size_t animationGen (const char* gif)
{
    assertStrict (gif, "received NULL");

    Descriptors descr = {};
    if (init (gif, &descr) != 0)
    {
        log_err ("init error", "something went wrong");
        return 0;
    }

    size_t frames = (size_t)fileSize (descr.source) / descr.srcBuf->size;
    log_string ("filesize: %ld, framesize: %llu, frames total: %zu\n", fileSize (descr.source), descr.srcBuf->size, frames);

    for (size_t f = 0; f < frames; f++)
    {
        framegen (descr.srcBuf, descr.dstBuf);
        log_string ("<ylw>Frame %zu was transfered<dft>\n", f);
    }
    bufWrite (descr.dstBuf, HALT, sizeof (HALT) - 1);

    log_string ("<grn>Work is done<dft>\n");
    release (&descr);

    return frames;
}