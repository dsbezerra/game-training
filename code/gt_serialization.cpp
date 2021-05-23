internal void
copy_memory(u8 *dest, u8 *src, u32 size) {
    for (u32 i = 0; i < size; ++i) {
        *dest++ = *src++;
    }
}

internal Byte_Buffer
make_byte_buffer(u32 capacity) {
    Byte_Buffer result = {};
    result.position = 0;
    result.size = 0;
    result.capacity = capacity;
    result.data = (u8 *) platform_alloc(result.capacity);
    return result;
}

internal void
destroy_byte_buffer(Byte_Buffer *buffer) {
    if (buffer->data) {
        platform_free(buffer->data);
    }
    buffer->position = 0;
    buffer->size = 0;
    buffer->capacity = BYTE_BUFFER_DEFAULT_CAPACITY;
}

internal void
byte_buffer_resize(Byte_Buffer *buffer, u32 capacity) {
    u8 *new_data = (u8 *) platform_alloc(capacity);
    if (new_data) {
        copy_memory(new_data, buffer->data, buffer->size);
        platform_free(buffer->data);
        
        buffer->data = new_data;
        buffer->capacity = capacity;
    }
}

internal void
byte_buffer_put(Byte_Buffer *buffer, void *data, u32 size) {
    assert (buffer->position + size < buffer->capacity);
    
    u8 *dest = buffer->data + buffer->position;
    copy_memory(dest, (u8 *) data, size);
    
    buffer->position += size;
    buffer->size += size;
}

internal Byte_Buffer
byte_buffer_from_file(char *filepath) {
    File_Contents file_data = platform_read_entire_file(filepath);
    assert(file_data.file_size > 0);
    
    Byte_Buffer result = {};
    result.data = file_data.contents;
    result.size = file_data.file_size;
    result.capacity = result.size;
    result.position = 0;
    
    return result;
}

internal b32
byte_buffer_write_to_file(Byte_Buffer *buffer, char *filepath) {
    if (!buffer || buffer->size == 0) return false;
    
    return platform_write_entire_file(filepath, buffer->data, buffer->size);
}

internal void
__BYTE_BUFFER_WRITE_IMPL(Byte_Buffer *buf, void *data, u32 size) {
    u32 total_write_size = buf->position + size;
    if (total_write_size >= buf->capacity) {
        u32 capacity = buf->capacity ? buf->capacity * 2 : BYTE_BUFFER_DEFAULT_CAPACITY;
        while (capacity < total_write_size) {
            capacity *= 2;
        }
        byte_buffer_resize(buf, capacity);
    }
    byte_buffer_put(buf, data, size);
}

#define BYTE_BUFFER_WRITE(buf, T, val) \
do { \
Byte_Buffer *_buf = (buf); \
T v = val; \
__BYTE_BUFFER_WRITE_IMPL(_buf, (void *)&(v), sizeof(T));\
} while (0)

#define BYTE_BUFFER_READ(_buf, T, val_ptr) \
do { \
T *val = (T*)val_ptr; \
Byte_Buffer *buf = _buf; \
*(val) = *(T*)(buf->data + buf->position); \
buf->position += sizeof(T); \
} while (0)

//
// Write functions
//

internal void
write_s8(Byte_Buffer *buffer, s8 value) {
    BYTE_BUFFER_WRITE(buffer, s8, value);
}

internal void
write_s16(Byte_Buffer *buffer, s16 value) {
    BYTE_BUFFER_WRITE(buffer, s16, value);
}

internal void
write_s32(Byte_Buffer *buffer, s32 value) {
    BYTE_BUFFER_WRITE(buffer, s32, value);
}

internal void
write_s64(Byte_Buffer *buffer, s64 value) {
    BYTE_BUFFER_WRITE(buffer, s64, value);
}

internal void
write_u8(Byte_Buffer *buffer, u8 value) {
    BYTE_BUFFER_WRITE(buffer, u8, value);
}

internal void
write_u16(Byte_Buffer *buffer, u16 value) {
    BYTE_BUFFER_WRITE(buffer, u16, value);
}

internal void
write_u32(Byte_Buffer *buffer, u32 value) {
    BYTE_BUFFER_WRITE(buffer, u32, value);
}

internal void
write_u64(Byte_Buffer *buffer, u64 value) {
    BYTE_BUFFER_WRITE(buffer, u64, value);
}

internal void
write_b32(Byte_Buffer *buffer, b32 value) {
    BYTE_BUFFER_WRITE(buffer, b32, value);
}

internal void
write_real32(Byte_Buffer *buffer, real32 value) {
    BYTE_BUFFER_WRITE(buffer, real32, value);
}

//
// Read functions
//

internal void
read_s8(Byte_Buffer *buffer, s8* value) {
    BYTE_BUFFER_READ(buffer, s8, value);
}

internal void
read_s16(Byte_Buffer *buffer, s16* value) {
    BYTE_BUFFER_READ(buffer, s16, value);
}

internal void
read_s32(Byte_Buffer *buffer, s32* value) {
    BYTE_BUFFER_READ(buffer, s32, value);
}

internal void
read_s64(Byte_Buffer *buffer, s64* value) {
    BYTE_BUFFER_READ(buffer, s64, value);
}

internal void
read_u8(Byte_Buffer *buffer, u8* value) {
    BYTE_BUFFER_READ(buffer, u8, value);
}

internal void
read_u16(Byte_Buffer *buffer, u16* value) {
    BYTE_BUFFER_READ(buffer, u16, value);
}

internal void
read_u32(Byte_Buffer *buffer, u32* value) {
    BYTE_BUFFER_READ(buffer, u32, value);
}

internal void
read_u64(Byte_Buffer *buffer, u64* value) {
    BYTE_BUFFER_READ(buffer, u64, value);
}

internal void
read_b32(Byte_Buffer *buffer, b32 *value) {
    BYTE_BUFFER_READ(buffer, b32, value);
}

internal void
read_real32(Byte_Buffer *buffer, real32 *value) {
    BYTE_BUFFER_READ(buffer, real32, value);
}
