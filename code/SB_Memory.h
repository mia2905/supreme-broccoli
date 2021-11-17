#ifndef SB_MEMORY_H
#define SB_MEMORY_H

#include "SB_Application.h"

struct MemoryPool
{
    memory_index size;
    u8*          base;
    memory_index usedBytes;
};

void* PushStruct_( MemoryPool* pool, memory_index size );

#define PushStruct( pool, struct ) (struct*)PushStruct_( pool, sizeof(struct) )
#define PushArray( pool, count, struct ) (struct*)PushStruct_( pool, (count)*sizeof(struct) )
#define PushBytes( pool, bytes ) (u8*)PushStruct_( pool, bytes )

#endif