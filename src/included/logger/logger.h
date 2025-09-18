#ifndef LOGGER_H
#define LOGGER_H


#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

#include "src/EscSeq.h"
#include "src/htmlTags.h"

#define SPEC_OPEN       '<'
#define SPEC_DIVIDER    ';'
#define SPEC_CLOSE      '>'


#define NAME_MAX 255

typedef uint64_t Erracc_t;

extern Erracc_t ErrAcc;

FILE* log_start (const char* fname);

int log_string (const char* format, ...);

void log_close();

const char* get_log();


#define print_stderr(class, description, ...)  fprintf\
    (\
        stderr,\
        "%s:%d: %s: " ESC_BOLD ESC_RED class ":" ESC_DEFAULT " " description ESC_BOLD_CLOSE "\n",\
        __FILE__,\
        __LINE__,\
        __func__,\
        ##__VA_ARGS__\
    )

#define log_err(class, description, ...)  log_string \
    (\
        "%s:%d: %s: <b><red>" class ":<dft> " description "</b>\n",\
        __FILE__,\
        __LINE__,\
        __func__,\
        ##__VA_ARGS__\
    );\
    log_string \
    (\
        "    | errAcc: %llu\n",\
        ErrAcc\
    )\


void memDump (const void* pointer, size_t byteSize);

unsigned long djb2Hash (const char* hashable, size_t size);

#endif