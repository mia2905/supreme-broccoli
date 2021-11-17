#include "SB_Memory.h"

void* PushStruct_( MemoryPool* pool, memory_index size )
{
    Assert( pool->usedBytes + size <= pool->size );
    u8* allocatedMemory = pool->base + pool->usedBytes;

    pool->usedBytes += size;

    return allocatedMemory;
}