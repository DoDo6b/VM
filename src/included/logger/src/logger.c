#include "../logger.h"


static char LogFile[NAME_MAX] = "";
static FILE* LogStream = NULL;

Erracc_t ErrAcc = 0;

FILE* log_start (const char* fname)
{
    if (!fname) print_stderr ("syntax error", "can't open stream");
    FILE* buffer = NULL;
    if      (!strncmp(fname, "stdout", NAME_MAX))   LogStream = stdout;
    else if (!strncmp(fname, "stderr", NAME_MAX))   LogStream = stderr;
    else
    {
        buffer = fopen (fname, "a+");
        if (!buffer)
        {
            print_stderr("internal error", "fopen returned a NULL");
            return NULL;
        }
        LogStream = buffer;

        atexit (log_close);

        fprintf (LogStream, "<pre>\n");
    }

    strncpy (LogFile, fname, NAME_MAX);

    return LogStream;
}


hash_t djb2Hash (const char* hashable, size_t size)
{
    if (!hashable)
    {
        print_stderr ("internal error", "can't get hash(NULL received)\n");
        return 0;
    }
    hash_t hash = 5381;
    
    for (; *hashable && size > 0; hashable++, size--)
    {
        hash = (hash << 5) + hash + (unsigned char)*hashable;
    }

    return hash;
}


enum SPECS_
{
    RESET        = 177655,      // <R>
    BOLD         = 177671,      // <b>
    BOLD_CLOSE   = 5861558,     // </b>
    ITALIC       = 177678,      // <i>
    ITALIC_CLOSE = 5861565,     // </i>

    BLACK        = 193487390,   // <blk>
    RED          = 193504576,   // <red>
    GREEN        = 193493036,   // <grn>
    YELLOW       = 193512449,   // <ylw>
    BLUE         = 193487400,   // <blu>
    MAGENTA      = 193499207,   // <mgn>
    CYAN         = 193488911,   // <cyn>
    WHITE        = 193510136,   // <wht>
    DEFAULT      = 193489379,   // <dft>
};

#define CASE_SPEC_(spec) case spec: fprintf(LogStream, ( (LogStream == stdout || LogStream == stderr) ? ESC_ ## spec : HTML_ ## spec) ); break

static void print_spec (const char* spec, size_t len)
{
    assert (spec);
    switch (djb2Hash (spec, len))
    {
        CASE_SPEC_(RESET);
        CASE_SPEC_(BOLD);
        CASE_SPEC_(BOLD_CLOSE);
        CASE_SPEC_(ITALIC);
        CASE_SPEC_(ITALIC_CLOSE);
        CASE_SPEC_(BLACK);
        CASE_SPEC_(RED);
        CASE_SPEC_(GREEN);
        CASE_SPEC_(YELLOW);
        CASE_SPEC_(BLUE);
        CASE_SPEC_(CYAN);
        CASE_SPEC_(MAGENTA);
        CASE_SPEC_(WHITE);
        CASE_SPEC_(DEFAULT);

        default:    fprintf(LogStream, "%c%s%c", SPEC_OPEN, spec, SPEC_CLOSE);
    }
}

#undef CASE_SPEC_

static size_t parse_spec (const char* substr)
{
    assert (substr && *substr == SPEC_OPEN);
    size_t collected = 0;

    substr++;

    for (collected = 0; substr[collected] != '\0' && substr[collected] != SPEC_CLOSE; collected++);
    print_spec (substr, collected);

    return collected + 2;
}

int log_string (const char* format, ...)
{
    if (!LogStream)
    {
        print_stderr("init error", "LogStream wasn't open, call log_start()");
        return -1;
    }
    if (!format) return 0;

    va_list args;
    va_start (args, format);

    char Buffer[BUFSIZ] = "";

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    vsnprintf (Buffer, sizeof (Buffer), format, args);
#pragma GCC diagnostic pop

    va_end (args);

    size_t printed = 0;
    for (  printed = 0; Buffer[printed] != '\0' && printed < sizeof(Buffer); printed++)
    {
        if (Buffer[printed] == SPEC_OPEN)   printed += parse_spec(&Buffer[printed]) - 1;
        else                                           fprintf(LogStream, "%c", Buffer[printed]);
    }

    if (LogStream != stderr || LogStream != stdout) fflush (LogStream);
    return (int)printed;
}


void log_close()
{
    if (LogStream && LogStream != stdout && LogStream != stderr) 
    {
        fprintf (LogStream, "\n</pre>\n");
        fclose (LogStream);
    }
}

const char* get_log()
{
    return LogFile;
}



void memDump (const void* pointer, size_t byteSize)
{
    const unsigned char* ptr        =   (const unsigned char*)pointer;

    log_string ("  Memory dump of 0x%p(%zu byte(s))\n", pointer, byteSize);
    log_string ("  {\n    ");
    
    log_string ("<blk>");
    for (size_t i = 0; i < byteSize; i++)
    {
        log_string ("%02zX ", i);
    }
    log_string ("<dft>\n    <cyn>");

    for (size_t i = 0; i < byteSize; i++)
    {
        log_string ("%02X ", *(ptr + i));
    }
        
    log_string ("<dft>\n  }\n");
}

void memBlockDump (const void* pointer, const void* highlight, size_t size, size_t width)
{
    const unsigned char* ptr = (const unsigned char*)pointer;

    log_string ("{\n  ");

    log_string ("                    ");
    log_string ("<blk>");
    for (size_t i = 0; i < width && i < size; i++) 
    {   
        if (i % 4 == 0) log_string (" ");
        log_string ("%02zX ", i);
    }
    log_string ("<dft>\n");

    for (size_t y = 0; y < size; y += width, ptr += width)
    {
        log_string ("  0x%p: ", ptr);

        log_string ("<cyn>");
        for (size_t x = 0; x < width && y + x < size; x++)
        {
            if (x % 4 == 0) log_string (" ");

            if (ptr + x != highlight) log_string ("%02X ", *(ptr + x));
            else                      log_string ("<dft><mgn><b>%02X</b><dft><cyn> ", *(ptr + x));
        }
        log_string ("<dft>\n");
    }
    
    log_string ("}\n");
}