#define push_struct(arena, type) (type *) push_size_(arena, sizeof(type))
#define push_array(arena, count, type) (type *) push_size_(arena, count*sizeof(type))
void *
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

#define kilobytes(value) ((value) * 1024)
#define megabytes(value) (kilobytes(value) * 1024)
#define gigabytes(value) (megabytes(value) * 1024)

#define zero_array(a) zero_size(sizeof(a), a)
#define zero_array_n(a,n) zero_size(sizeof(a)*n, a)
#define zero_struct(s) zero_size(sizeof(s), &(s))

internal void
init_arena(Memory_Arena *arena, Memory_Index size, void *base) {
    arena->size = size;
    arena->base = (u8 *) base;
    arena->used = 0;
}

internal void
reset_arena(Memory_Arena *arena) {
    if (arena->used == 0) return;
    
    zero_size(arena->used, arena->base);
    
    arena->used = 0;
}
