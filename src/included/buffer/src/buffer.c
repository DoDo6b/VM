#include "../buffer.h"

Buffer* bufInit (size_t size)
{
    if (size == 0) return NULL;

    Buffer* buf = (Buffer*)calloc(1, sizeof (Buffer));

    void* ptr = calloc (size, sizeof (char));
    if (!ptr)
    {
        log_err ("internal error", "calloc returned NULL");
        return NULL;
    }

    buf->mode = 0;
    buf->buffer = (char*)ptr;
    buf->bufpos = buf->buffer;
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
    buf->bufpos = buf->buffer;
    
    #ifdef SECURE
    memset (buf->buffer, 0, buf->size);
    #endif
    
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
    assertStrict (buf, "received NULL");
    assertStrict (buf->stream, "stream to flush buffer wasnt opened or wasnt linked");

    switch (buf->mode)
    {
        case 'w':
            fwrite (buf->buffer, buf->len, 1, buf->stream);

            #ifdef SECURE
            memset (buf->buffer, 0, buf->size);
            #endif
            break;

        case 'r':
            #ifdef SECURE
            memset (buf->buffer, 0, buf->size);
            #endif

            break;

        default: return 0;
    }

    size_t flushed = buf->len;
    buf->len = 0;
    buf->bufpos = buf->buffer;

    return flushed;
}


size_t bufRead (Buffer* buf, size_t size)
{
    assertStrict (buf, "received NULL");

    assertStrict (buf->mode == 'r', "wrong buffer mode");
    assertStrict (buf->stream, "stream wasnt opened or wasnt linked");

    buf->bufpos = buf->buffer + buf->len;       // логично ли? хз, как будто не всегда стоит переносить указатель на начало новопрочитанного

    if (size != 0)
    {
        long chRemains = ftell (buf->stream);
        chRemains = fileSize (buf->stream) - chRemains;
        
        size = ((size_t)chRemains > size) ? size : chRemains;

        if (size > buf->size - buf->len)
        {
            log_err ("warning", "buffer is about to overflow");
            size = buf->size - buf->len;
        }
    }
    else size = buf->size - buf->len;
    
    fread (buf->bufpos, size, 1, buf->stream);
    buf->len += size;

    return size;
}

size_t bufWrite (Buffer* buf, void* src, size_t size)
{
    assertStrict (buf, "received NULL");
    assertStrict (src, "received NULL");

    assertStrict (buf->mode == 'w', "wrong buffer mode");

    if (buf->bufpos + size > buf->buffer + buf->size) if (bufFlush (buf) == 0) return 0;

    memcpy (buf->bufpos, src, size);
    buf->bufpos += size;
    if (buf->bufpos - buf->buffer > (ssize_t)buf->len) buf->len = buf->bufpos - buf->buffer;

    return size;
}