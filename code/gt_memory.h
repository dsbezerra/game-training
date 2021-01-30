/* date = January 30th 2021 0:01 am */

typedef size_t Memory_Index;

struct Memory_Arena {
    Memory_Index size;
    Memory_Index used;
    u8 *base;
};

internal void init_arena(Memory_Arena *arena, Memory_Index size, void *base);
internal void reset_arena(Memory_Arena *arena);