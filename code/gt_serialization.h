/* date = May 22nd 2021 10:58 pm */

#define BYTE_BUFFER_DEFAULT_CAPACITY 1024

struct Byte_Buffer {
    u8 *data;
    u32 position;
    u32 size;
    u32 capacity;
};

internal Byte_Buffer make_byte_buffer(u32 capacity = BYTE_BUFFER_DEFAULT_CAPACITY);
internal void        destroy_byte_buffer(Byte_Buffer *buffer);

internal void byte_buffer_resize(Byte_Buffer *buffer, u32 capacity);
internal void byte_buffer_put(Byte_Buffer *buffer, void *data, u32 size);

internal Byte_Buffer byte_buffer_from_file(char *filepath);
internal b32 byte_buffer_write_to_file(Byte_Buffer *buffer, char *filepath);