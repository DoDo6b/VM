#include "framegen.h"

typedef struct
{
    FILE*   source;
    Buffer* srcBuf;
    FILE*   destination;
    Buffer* dstBuf;
}Descriptors;


static int init (const char* gif, int w, int h, Descriptors* descr)
{
    assertStrict (gif, "received NULL");

    descr->source = fopen (gif, "r");
    if (descr->source == NULL)
    {
        log_err ("fs error", "cant open animation");
        return -1;
    }

    descr->srcBuf = bufInit ((size_t)(w * h));
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


static void framegen (Buffer* bufR, Buffer* bufW, int w, int h)
{
    assertStrict (bufVerify (bufR, 0) == 0 && bufR->mode == BUFREAD,  "bufR failed verification");
    assertStrict (bufVerify (bufW, 0) == 0 && bufW->mode == BUFWRITE, "bufW failed verification");
    assertStrict (w > 0 && h > 0, "w < 0 || h < 0");

    bufFlush (bufR);
    if (bufRead  (bufR, w * h) != (size_t)(w * h))
    {
        log_err ("warning", "cropped frame detected");
    }

    char buf[16] = {0};
    for (int i = 0; i < w * h; i++)
    {
        sprintf (buf, "%d", (int)bufGetc (bufR));

        bufWrite (bufW, PUSH,   sizeof (PUSH) - 1);
        bufWrite (bufW, buf,    strlen (buf));
        bufWrite (bufW, "\n",   sizeof ("\n") - 1);

        bufWrite (bufW, MOV,    sizeof (MOV) - 1);
        bufWrite (bufW, "[",    sizeof ("[") - 1);

        sprintf (buf, "%d", i);
        bufWrite (bufW, buf,    strlen (buf));
        
        bufWrite (bufW, "]\n",  sizeof ("]\n") - 1);
    }

    bufWrite (bufW, DRAW, sizeof (DRAW) - 1);
}


size_t animationGen (const char* gif, int w, int h)
{
    assertStrict (gif, "received NULL");
    assertStrict (w > 0 && h > 0, "w < 0 || h < 0");

    Descriptors descr = {0};
    if (init (gif, w, h, &descr) != 0)
    {
        log_err ("init error", "something went wrong");
        return 0;
    }

    size_t frames = (unsigned long)fileSize (descr.source) / (unsigned long)(w * h);
    log_string ("filesize: %lu, framesize: %lu, frames total: %zu\n", fileSize (descr.source), (unsigned long)(w * h), frames);

    for (size_t f = 0; f < frames; f++)
    {
        framegen (descr.srcBuf, descr.dstBuf, w, h);
        log_string ("<ylw>Frame %llu was transfered<dft>\n", f);
    }
    bufWrite (descr.dstBuf, HALT, sizeof (HALT) - 1);

    log_string ("<grn>Work is done<dft>\n");
    release (&descr);

    return frames;
}