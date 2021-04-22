/* date = January 30th 2021 0:01 am */

typedef size_t Memory_Index;

struct Memory_Arena {
    Memory_Index size;
    u8 *base;
    Memory_Index used;
    
    int32 temp_count;
};

struct Temporary_Memory {
    Memory_Arena *arena;
    Memory_Index used;
};

internal void init_arena(Memory_Arena *arena, Memory_Index size, void *base);
internal void reset_arena(Memory_Arena *arena);