#include "../buffer.h"

void bufInit (Buffer* buf, size_t size)
{
    void* ptr = calloc (sizeof (char), size);
    if (!ptr)
    {
        log_err ("internal error", "calloc returned NULL");
        exit (EXIT_FAILURE);
    }

    buf->buffer = (char*)ptr;
    buf->len = 0;
    buf->size = size;
    buf->stream = NULL;
}

void bufSetStream (Buffer* buf, FILE* stream)
{
    if (!stream)
    {
        log_err ("runtime error", "stream wasnt opened");
        exit (EXIT_FAILURE);
    }

    buf->stream = stream;
}

void bufFree (Buffer* buf)
{
    bufFlush (buf);
    free (buf->buffer);

    #ifdef SECURE
    memset (buf, 0XCC, sizeof (Buffer));
    #endif
}

size_t bufFlush (Buffer* buf)
{
    if (!buf->stream)
    {
        log_err ("runtime error", "stream to flush buffer wasnt opened");
        exit (EXIT_FAILURE);
    }

    size_t wrote = fwrite (buf->buffer, buf->len, 1, buf->stream);

    #ifdef SECURE
    memset (buf->buffer, 0, buf->size);
    #endif

    buf->len = 0;

    return wrote;
}

size_t bufWrite (Buffer* buf, void* src, size_t size)
{
    if (buf->len + size > buf->size) bufFlush (buf);
    memcpy (buf->buffer + buf->len, src, size);
    buf->len += size;
    return size;
}