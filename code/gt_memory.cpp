#define push_struct(arena, type) (type *) push_size_(arena, sizeof(type))
#define push_array(arena, count, type) (type *) push_size_(arena, count*sizeof(type))
inline void *
push_size_(Memory_Arena *arena, Memory_Index size) {
    assert((arena->used + size) <= arena->size);
    
    void *result = arena->base + arena->used;
    arena->used += size;
    
    return result;
}

inline void
zero_size(Memory_Index size, void *pointer) {
    u8* byte = (u8*) pointer;
    while (--size) {
        *byte++ = 0;
    } 
}

#define kilobytes(value) ((value) * 1024LL)
#define megabytes(value) (kilobytes(value) * 1024LL)
#define gigabytes(value) (megabytes(value) * 1024LL)

#define zero_array(a) zero_size(sizeof(a), a)
#define zero_array_n(a,n) zero_size(sizeof(a)*n, a)
#define zero_struct(s) zero_size(sizeof(s), &(s))

inline void
init_arena(Memory_Arena *arena, Memory_Index size, void *base) {
    arena->size = size;
    arena->base = (u8 *) base;
    arena->used = 0;
    arena->temp_count = 0;
}

inline void
reset_arena(Memory_Arena *arena) {
    if (arena->used == 0) return;
    
    zero_size(arena->used, arena->base);
    
    arena->used = 0;
}

inline Temporary_Memory
begin_temporary_memory(Memory_Arena *arena) {
    Temporary_Memory result;
    
    result.arena = arena;
    result.used = arena->used;
    
    ++arena->temp_count;
    
    return result;
}

inline void
end_temporary_memory(Temporary_Memory temp_mem) {
    Memory_Arena *arena = temp_mem.arena;
    assert(arena->used >= temp_mem.used);
    arena->used = temp_mem.used;
    assert(arena->temp_count > 0);
    --arena->temp_count;
}

inline void
check_arena(Memory_Arena *arena) {
    assert(arena->temp_count == 0);
}
