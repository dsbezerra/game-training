/* date = September 7th 2020 5:44 pm */

struct file_contents {
    u32 file_size;
    u8 *contents;
};

internal void *platform_alloc(u64 size);
internal void platform_free(void *memory);
internal void platform_show_cursor(b32 show);
internal void platform_set_cursor_position(v2i position);
internal void platform_get_cursor_position(v2i *position);
internal file_contents platform_read_entire_file(char *filepath);

internal u64 platform_get_perf_counter();
internal real32 platform_seconds_elapsed(u64 last_counter);
