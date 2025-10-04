#include "../buffer.h"

Buffer* bufInit (size_t size)
{
    if (size == 0) return NULL;

    Buffer* buf = (Buffer*)calloc(1, sizeof (Buffer));

    void* ptr = calloc (size + 1, sizeof (char));
    if (!ptr)
    {
        log_err ("internal error", "calloc returned NULL");
        return NULL;
    }

    buf->mode =     NOTSETTED;
    buf->buffer =   (char*)ptr;
    buf->bufpos =   buf->buffer;
    buf->len =      0;
    buf->size =     size;
    buf->stream =   NULL;
    buf->name =     NULL;

    assertStrict (bufVerify (buf, BUFDETACHED | BUFFACCESS) == 0, "failed buffer verification");

    return buf;
}

void bufFree (Buffer* buf)
{
    Erracc_t vericode = bufVerify (buf, 0);

    if (vericode == 0) bufFlush (buf);
    if ((vericode & BUFDETACHED) == 0) free (buf->buffer);

    if ((vericode & BUFNULL) == 0)
    {
        #ifdef SECURE
        memset (buf, 0XCC, sizeof (Buffer));
        #endif

        free (buf);
    }
}

Erracc_t bufVerify (Buffer* buf, Erracc_t ignored)
{
    if (buf == NULL)
    {
        log_err ("verification error", "received NULL");
        ErrAcc |= BUF_ERRCODE (BUFNULL);
        return ErrAcc;
    }

    if ((ignored & BUFNOTINITED) == 0 && (buf->buffer == NULL || buf->size == 0))
    {
        ErrAcc |= BUF_ERRCODE (BUFNOTINITED);
        log_err ("verification error", "buffer wasnt initialized");
    }

    if ((ignored & BUFOVERFLOW) == 0 && (buf->bufpos < buf->buffer || buf->buffer + buf->size < buf->bufpos || buf->len > buf->size))
    {
        ErrAcc |= BUF_ERRCODE (BUFOVERFLOW);
        log_err ("verification error", "buffer overflow");
    }

    if ((ignored & BUFDETACHED) == 0 && (buf->stream == NULL))
    {
        ErrAcc |= BUF_ERRCODE (BUFDETACHED);
        log_err ("verification error", "buffer detached (file stream is not linked)");
    }

    if ((ignored & BUFFACCESS) == 0 && (buf->mode != BUFWRITE && buf->mode != BUFREAD))
    {
        ErrAcc |= BUF_ERRCODE (BUFFACCESS);
        log_err ("verification error", "stream mode is not setted or wrong");
    }

    return ErrAcc;
}


int bufSetStream (Buffer* buf, const char* name, FILE* stream, BufMode_t mode)
{
    assertStrict (bufVerify (buf, BUFDETACHED | BUFFACCESS) == 0, "failed buffer verification");

    if (!stream)
    {
        log_err ("runtime error", "stream wasnt opened");
        
        return -1;
    }

    buf->stream = stream;
    buf->bufpos = buf->buffer;
    buf->name = name;
    
    #ifdef SECURE
    memset (buf->buffer, 0, buf->size);
    #endif
    
    buf->mode = mode;

    return 0;
}

FILE* bufFOpen (Buffer* buf, const char* fname, const char* mode)
{
    assertStrict (bufVerify (buf, BUFDETACHED | BUFFACCESS) == 0, "failed buffer verification");
    assertStrict (fname, "received NULL");
    assertStrict (mode,  "received NULL");

    FILE* stream = fopen (fname, mode);
    if (!stream)
    {
        log_err ("runtime error", "cant open stream");
        
        return NULL;
    }

    const char* w = strchr(mode, 'w');
    const char* a = (!w) ? strchr(mode, 'a') : NULL;
    const char* r = strchr(mode, 'r');

IF_DBG ( if ((w || a) && r) log_err ("warning", "buffer was opened with 'w' flag [Dual-purpose]"); )

    if (w || a) bufSetStream (buf, fname, stream, BUFWRITE);
    else        bufSetStream (buf, fname, stream, BUFREAD);

    return stream;
}


size_t bufFlush (Buffer* buf)
{
    assertStrict (bufVerify (buf, 0) == 0, "failed buffer verification");

    switch (buf->mode)
    {
        case NOTSETTED: return 0;

        case BUFWRITE:
            fwrite (buf->buffer, buf->len, 1, buf->stream);

            #ifdef SECURE
            memset (buf->buffer, 0, buf->size);
            #endif
            break;

        case BUFREAD:
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
    assertStrict (bufVerify (buf, 0) == 0, "failed buffer verification");
    assertStrict (buf->mode == BUFREAD, "incompatible buffer mode");

    if (size != 0)
    {
        long chRemains = ftell (buf->stream);
        if (chRemains >= 0) 
        chRemains = fileSize (buf->stream) - chRemains;
        
        size = ((size_t)chRemains > size) ? size : (size_t)chRemains;

        if (size > buf->size - buf->len)
        {
            log_err ("warning", "buffer is about to overflow");
            size = buf->size - buf->len;
        }
    }
    else size = buf->size - buf->len;
    
    size_t read = fread (buf->buffer + buf->len, size, 1, buf->stream);
    buf->len += read;

    return size;
}

size_t bufWrite (Buffer* buf, void* src, size_t size)
{
    assertStrict (bufVerify (buf, 0) == 0, "failed buffer verification");
    assertStrict (buf->mode == BUFWRITE, "incompatible buffer mode");

    if (buf->bufpos + size > buf->buffer + buf->size) if (bufFlush (buf) == 0) return 0;

    memcpy (buf->bufpos, src, size);
    buf->bufpos += size;
    if (buf->bufpos - buf->buffer > (ssize_t)buf->len) buf->len = (size_t)(buf->bufpos - buf->buffer);

    return size;
}


int bufScanf (Buffer* buf, const char* format, void* dst)
{
    assertStrict (bufVerify (buf, 0) == 0, "failed buffer verification");
    assertStrict (buf->mode == BUFREAD, "incompatible buffer mode");

    size_t  oldFormatL = strlen (format);
    char*   newFormat = (char*)calloc (oldFormatL + sizeof ("%n\0"), sizeof (char));

    memcpy (newFormat, format, oldFormatL);
    memcpy (newFormat + oldFormatL, "%n", sizeof ("%n"));

    size_t read = 0;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    int scanned = sscanf (buf->bufpos, newFormat, dst, &read);
#pragma GCC diagnostic pop
    
    if (buf->bufpos + read > buf->buffer + buf->size)
    {
        ErrAcc |= BUF_ERRCODE (BUFOVERFLOW);
        log_err ("runtime error", "buffer position pointer is out of bounds");
        return -1;
    }
    buf->bufpos += read;

    return scanned;
}

void bufSpaces (Buffer* buf)
{
    assertStrict (bufVerify (buf, 0) == 0, "buffer failed verification");
    assertStrict (buf->mode == BUFREAD, "incompatible buffer mode");

    while (*buf->bufpos == ' ' && buf->bufpos < buf->buffer + buf->size) buf->bufpos++;
}

char bufGetc (Buffer* buf)
{
    assertStrict (bufVerify (buf, 0) == 0, "buffer failed verification");

    char c = *buf->bufpos;
    buf->bufpos++;
    return c;
}

char bufpeekc (Buffer* buf)
{
    assertStrict (bufVerify (buf, 0) == 0, "buffer failed verification");

    return *buf->bufpos;
}

long long bufSeek (Buffer* buf, long offset, char base)
{
    assertStrict (bufVerify (buf, 0) == 0, "buffer failed verification");

    switch (base)
    {
    case SEEK_CUR:
        buf->bufpos += offset;
        if      (buf->bufpos < buf->buffer) buf->bufpos = buf->buffer;
        else if (buf->bufpos > buf->buffer + buf->size) buf->bufpos = buf->buffer + buf->size;
        break;

    case SEEK_SET:
        buf->bufpos = buf->buffer + offset;
        if      (buf->bufpos < buf->buffer) buf->bufpos = buf->buffer;
        else if (buf->bufpos > buf->buffer + buf->size) buf->bufpos = buf->buffer + buf->size;
        break;

    case SEEK_END:
        buf->bufpos = buf->buffer + buf->size + offset;
        if      (buf->bufpos < buf->buffer) buf->bufpos = buf->buffer;
        else if (buf->bufpos > buf->buffer + buf->size) buf->bufpos = buf->buffer + buf->size;
        break;

    default:
        ErrAcc |= BUF_ERRCODE (BUFSYNTAX);
        log_err ("syntax error", "unknown base");
    }

    return buf->bufpos - buf->buffer;
}

long bufTell (Buffer* buf)
{
    return (long)(buf->bufpos - buf->buffer);
}

void bufCpy (Buffer* buf, void* dst, size_t size)
{
    assertStrict (bufVerify (buf, 0) == 0, "buffer failed verification");
    assertStrict (dst, "received NULL");

    if (buf->bufpos + size < buf->buffer + buf->size)
    {
        memcpy (dst, buf->bufpos, size);
        buf->bufpos += size;
    }
}