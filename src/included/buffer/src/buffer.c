#include "../buffer.h"

Buffer* bufInit (size_t size)
{
    if (size == 0) return NULL;

    Buffer* buf = (Buffer*)calloc(1, sizeof (Buffer));

    void* ptr = calloc (sizeof (char), size);
    if (!ptr)
    {
        log_err ("internal error", "calloc returned NULL");
        return NULL;
    }

    buf->mode = 0;
    buf->buffer = (char*)ptr;
    buf->len = 0;
    buf->size = size;
    buf->stream = NULL;

    return buf;
}

int bufSetStream (Buffer* buf, FILE* stream, char mode)
{
    assertStrict (buf, "received NULL");

    if (!stream)
    {
        log_err ("runtime error", "stream wasnt opened");
        return -1;
    }

    buf->stream = stream;
    
    if (mode == 'w' || mode == 'r') buf->mode = mode;
    else
    {
        log_err ("attribute error", "incompatible mode");
        return -1;
    }

    return 0;
}

void bufFree (Buffer* buf)
{
    assertStrict (buf, "received NULL");

    bufFlush (buf);
    free (buf->buffer);

    #ifdef SECURE
    memset (buf, 0XCC, sizeof (Buffer));
    #endif

    free (buf);
}

size_t bufFlush (Buffer* buf)
{
    assertStrict (buf->stream, "stream to flush buffer wasnt opened");

    size_t wrote = fwrite (buf->buffer, buf->len, 1, buf->stream);

    if (wrote > 0)
    {
        #ifdef SECURE
        memset (buf->buffer, 0, buf->size);
        #endif

        buf->len = 0;
    }

    return wrote;
}

size_t bufWrite (Buffer* buf, void* src, size_t size)
{
    assertStrict (buf->mode == 'w', "wrong buffer mode");

    if (buf->len + size > buf->size) if (bufFlush (buf) == 0) return 0;

    memcpy (buf->buffer + buf->len, src, size);
    buf->len += size;

    return size;
}