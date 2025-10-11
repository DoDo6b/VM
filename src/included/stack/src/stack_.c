#include "stack_.h"


T2
(
static inline void updateT2Hashes (Stack* dst)
{
    assertStrict (dst,       "recived a NULL");
    assertStrict (dst->data, "stack wasnt initialized");

    dst->crc32Data  = crc32Calculate ((unsigned char*)dst->data, dst->capacity * dst->sizeOfElem);
    dst->crc32      = crc32Calculate ((unsigned char*)dst, (size_t)(&dst->crc32 - &dst->frontCanary));
}
)

static void updateMetaInfo (Stack* stack)
{
    assertStrict (stack, "recived a NULL");
T1( assertStrict (stack->frontCanary == FRONTCANARY && stack->tailCanary == TAILCANARY, "stack signes corrupted"); )
    assertStrict (stack->data && stack->sizeOfElem > 0 && stack->capacity > 0, "struct corrupted");
    
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"

T1  (
    uintptr_t* frontOffset = (uintptr_t*)stack->data - 1;
              *frontOffset = (uintptr_t) stack->data ^ HEXSPEAK;

    uintptr_t* tailOffset = (uintptr_t*)(stack->data + stack->capacity * stack->sizeOfElem  + (stack->capacity * stack->sizeOfElem % sizeof (uintptr_t)) );
              *tailOffset = (uintptr_t) (stack->data + stack->capacity * stack->sizeOfElem) ^ HEXSPEAK;
    )

#pragma GCC diagnostic pop

T2( updateT2Hashes (stack); )

    assertStrict (stackVerifyD (stack) == 0, "verification failed, cant continue");
}


Stack* stackInitD (size_t numOfElem, ssize_t sizeOfElem)
{
    Stack* dst = (Stack*)calloc(1, sizeof(Stack));
    assertStrict (dst, "calloc returned NULL");
    if (!dst) return NULL;

    assertStrict (numOfElem > 0 && sizeOfElem > 0, "cant allocate stack with capacity 0 or element size equal 0");

    size_t reservedMemory = 0 T1 ( + 2 * sizeof (uintptr_t) + numOfElem * (size_t)sizeOfElem % sizeof (uintptr_t) );
    dst->data = (char*) calloc (1, numOfElem * (size_t)sizeOfElem + reservedMemory) T1 ( + sizeof (uintptr_t));
    assertStrict (dst->data != (void*)(0 T1 ( + sizeof (uintptr_t))), "calloc returned NUL");

    if (dst->data)
    {
        dst->top        = dst->data;
        dst->sizeOfElem = (size_t)sizeOfElem;
        dst->capacity   = numOfElem;

T1      (
        dst->frontCanary = FRONTCANARY;
        dst->tailCanary  = TAILCANARY;

        updateMetaInfo (dst);
        )

        assertStrict (stackVerifyD (dst) == 0, "verification failed, cant continue");

        return dst;
    }
    else
    {
        free (dst);
        return NULL;
    }
}

void stackReallocD (Stack* stack, size_t newCapacity, bool ignoreDataLoss)
{
    assertStrict (stackVerifyD (stack) == 0, "verification failed, cant continue");

    if (stack->capacity == newCapacity) return;

    assertStrict (newCapacity > stack->capacity ||  ignoreDataLoss || (size_t)(stack->top - stack->data) / stack->sizeOfElem < newCapacity, "data loss");
    if (          newCapacity < stack->capacity && !ignoreDataLoss && (size_t)(stack->top - stack->data) / stack->sizeOfElem > newCapacity) return;

    size_t reservedMemory = 0 T1 ( + 2 * sizeof (uintptr_t) + newCapacity * stack->sizeOfElem % sizeof (uintptr_t) );

    size_t newSize        = newCapacity * stack->sizeOfElem + reservedMemory;
    size_t topOffset      = (size_t)(stack->top - stack->data);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"

T1  (
S1  (
    uintptr_t* frontOffset = (uintptr_t*)stack->data - 1;
              *frontOffset = 0XCCCCCCCCCCCCCCCC;

    uintptr_t* tailOffset = (uintptr_t*)(stack->data + stack->capacity * stack->sizeOfElem  + (stack->capacity * stack->sizeOfElem % sizeof (uintptr_t)) );
              *tailOffset = 0XCCCCCCCCCCCCCCCC;
    )
    )

#pragma GCC diagnostic pop

    char*         newBlock = (char*)realloc (stack->data T1 ( - sizeof (uintptr_t) ), newSize);
    assertStrict (newBlock, "realloc returned NULL");
    if (         !newBlock) return;

    stack->data     = newBlock T1 ( + sizeof (uintptr_t) );
    if (newCapacity > stack->capacity) memset (stack->data + stack->capacity * stack->sizeOfElem, 0, (newCapacity - stack->capacity) * stack->sizeOfElem);
    stack->capacity = newCapacity;
    stack->top      = stack->data + topOffset;

T1( updateMetaInfo (stack); )

    assertStrict (stackVerifyD (stack) == 0, "verification failed, cant continue");
}

void stackFreeD (Stack* stack)
{
    assertStrict (stackVerifyD (stack) == 0, "verification failed, cant continue");
T1( stack->data -= sizeof (uintptr_t); )

S1( memset (stack->data, 0XCC, stack->capacity * stack->sizeOfElem); )
    free   (stack->data);

    memset (stack,       0xCC, sizeof (Stack));
    free   (stack);
}


void stackTopD (const Stack* stack, void* dst)
{
    assertStrict (stackVerifyD (stack) == 0, "verification failed, cant continue");
    
    memcpy (dst, stack->top - stack->sizeOfElem, stack->sizeOfElem);
}

void stackPushD (Stack* stack, const void* src)
{
    assertStrict (stackVerifyD (stack) == 0, "verification failed, cant continue");
    assertStrict (src, "received a NULL");

#ifdef AREALLOC
    if ((size_t)(stack->top - stack->data) >= stack->capacity * stack->sizeOfElem) stackReallocD (stack, stack->capacity * GrowthRate / 100, false);
#endif
    if ((size_t)(stack->top - stack->data) <  stack->capacity * stack->sizeOfElem)
    {
        memcpy (stack->top, src, stack->sizeOfElem);
                stack->top +=    stack->sizeOfElem;

T2(     updateT2Hashes (stack); )

        assertStrict (stackVerifyD (stack) == 0, "verification failed, cant continue");
    }
}

void stackPopD_ (Stack* stack, void* dst /* = NULL */)
{
    assertStrict (stackVerifyD (stack) == 0, "verification failed, cant continue");

    if (stack->top -  stack->sizeOfElem >= stack->data)
    {
        stack->top -= stack->sizeOfElem;

        if (dst) memcpy (dst, stack->top, stack->sizeOfElem);

S1(     memset (stack->top, 0XCC, stack->sizeOfElem); )
T2(     updateT2Hashes (stack); )

#ifdef AREALLOC
        if ((stack->top - stack->data) / stack->sizeOfElem < stack->capacity * ReductionRate / 100) stackReallocD (stack, stack->capacity * ReductionRate / 100, false);
#endif

        assertStrict (stackVerifyD (stack) == 0, "verification failed, cant continue");
    }
}

size_t stackLenD (const Stack* stack)
{
    assertStrict (stackVerifyD (stack) == 0, "verification failed, cant continue");
    return (size_t)(stack->top - stack->data) / stack->sizeOfElem;
}


void stackDumpD_ (const char* name, const Stack* stack, void (*print)(const void* obj))
{
    if (stackVerifyD (stack) != 0)
    {
        log_err ("internal error", "verification failed, cant continue");
        return;
    }

    log_string
    (
        "<blu>%s dump:<dft>\n"
T1(     "frontCanary:     0X%0X\n" )
        "top ptr:         %p\n"
        "data block ptr:  %p\n"
        "capacity:        %zu elem(s)\n"
        "size of element: %zu byte(s)\n"
T2(     "crc of data:     %u\n" )
T2(     "crc of struct:   %u\n" )
T1(     "tailCanary:      0X%0X\n" )
        "real size        %zu byte(s)\n"
        "{\n",
        name,
T1(     stack->frontCanary, )
        stack->top,
        stack->data,
        stack->capacity,
        stack->sizeOfElem,
T2(     stack->crc32Data, )
T2(     stack->crc32, )
T1(     stack->tailCanary, )
        stack->top - stack->data
    );

T1  (
    log_string ("  [frontCanary]: ");
    memDump (stack->data - sizeof(uintptr_t), sizeof(uintptr_t));
    )

    for (size_t i = 0; stack->data + i <= stack->top; i += stack->sizeOfElem)
    {
        log_string ("  [%zu]: ", i / stack->sizeOfElem);
        
        if (print) print (stack->data + i);
        else       memDump (stack->data + i, stack->sizeOfElem);
    }

T1  (
    log_string ("  [tailCanary]: ");
    memDump (stack->data + stack->sizeOfElem * stack->capacity + (stack->sizeOfElem * stack->capacity) % sizeof (uintptr_t), sizeof (uintptr_t));
    )

    log_string ("}\n");
}


Erracc_t stackVerifyD_ (const char* callerFile, unsigned int callerLine, const Stack* stack)
{
    if (stack == NULL)
    {
        log_string
        (
            "%s:%d: %s: <b><red>verification error:<dft> received a NULL</b>\n",
            callerFile,
            callerLine,
            __func__
        );
        ErrAcc |= STCK_ERRCODE (RECIVED_NULL);
        log_err ("verification error", "failed");
        return ErrAcc;
    }

T1  (
    if (stack->frontCanary != FRONTCANARY || stack->tailCanary != TAILCANARY)
    {
        log_string
        (
            "%s:%d: %s: <b><red>verification error:<dft> main signes was corrupted or its not a Stack</b>\n",
            callerFile,
            callerLine,
            __func__
        );
        ErrAcc |= STCK_ERRCODE (MAIN_SIGNES_CORRUPTED);
        log_err ("verification error", "failed");
        return ErrAcc;
    }
    )

    if (stack->data == NULL || stack->top == NULL || stack->sizeOfElem == 0 || stack->capacity == 0)
    {
        log_string
        (
            "%s:%d: %s: <b><red>verification error:<dft> stack wasnt initilized</b>\n",
            callerFile,
            callerLine,
            __func__
        );
        ErrAcc |= STCK_ERRCODE (NOT_INITILIZED);
        log_err ("verification error", "failed");
        return ErrAcc;
    }

    if (stack->top < stack->data || stack->top > stack->data + stack->capacity * stack->sizeOfElem || (size_t)(stack->top - stack->data) % stack->sizeOfElem != 0)
    {
        log_string
        (
            "%s:%d: %s: <b><red>verification error:<dft> top pointer is in am impossible position</b>\n",
            callerFile,
            callerLine,
            __func__
        );
        ErrAcc |= STCK_ERRCODE (TOPPTR_OUTOFBOUNDS);
    }

T2  ( 
    if (stack->crc32 != crc32Calculate ((const unsigned char*)stack, (size_t)(&stack->crc32 - &stack->frontCanary)))
    {
        log_string
        (
            "%s:%d: %s: <b><red>verification error:<dft> crc32 hash of struct has changed</b>\n",
            callerFile,
            callerLine,
            __func__
        );
        ErrAcc |= STCK_ERRCODE (CRCMAIN_HASCHANGED);
    }
    )

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"

T1  (
    uintptr_t* frontOffset = (uintptr_t*)stack->data - 1;
    uintptr_t* tailOffset = (uintptr_t*)(stack->data + stack->capacity * stack->sizeOfElem + ((stack->capacity * stack->sizeOfElem) % sizeof (uintptr_t)) );

    if ( *frontOffset != ((uintptr_t) stack->data ^ HEXSPEAK) || *tailOffset != ((uintptr_t)(stack->data + stack->capacity * stack->sizeOfElem) ^ HEXSPEAK) )
    {
        log_string
        (
            "%s:%d: %s: <b><red>verification error:<dft> data signes was corrupted</b>\n",
            callerFile,
            callerLine,
            __func__
        );
        ErrAcc |= STCK_ERRCODE (DATABLOCK_SIGNES_CORRUPTED);
    }
    )

#pragma GCC diagnostic pop

T2  (
    if (stack->crc32Data != crc32Calculate ((const unsigned char*)stack->data, stack->capacity * stack->sizeOfElem))
    {
        log_string
        (
            "%s:%d: %s: <b><red>verification error:<dft> crc32 hash of data has changed</b>\n",
            callerFile,
            callerLine,
            __func__
        );
        ErrAcc |= STCK_ERRCODE (CRCDATA_HASHCHANGED);
    }
    )


    if (ErrAcc != 0)
    {
        log_err ("verification error", "failed");
    }
    return ErrAcc;
}