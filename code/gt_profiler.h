/* date = September 27th 2020 1:34 pm */

enum profiler_item {
    ProfilerItem_Input,
    ProfilerItem_Count,
};

struct profiling_data {
    u64 hit_count;
    u64 begin_timer;
    real32 timer;
};

struct profiling_font {
    loaded_font font;
    b32 loaded;
};
