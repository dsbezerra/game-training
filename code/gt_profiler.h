/* date = September 27th 2020 1:34 pm */

enum Profiler_Item {
    ProfilerItem_Input,
    ProfilerItem_GameUpdateAndRender,
    ProfilerItem_Count,
};

struct Profiling_Data {
    u64 hit_count;
    u64 begin_timer;
    real32 timer;
};

struct Profiling_Font {
    Loaded_Font font;
    b32 loaded;
};
