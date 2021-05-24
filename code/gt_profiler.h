/* date = September 27th 2020 1:34 pm */

enum Profiler_Item {
    ProfilerItem_Input,
    ProfilerItem_GameUpdateAndRender,
    ProfilerItem_Audio,
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

global_variable Profiling_Data global_back_profiling_data[ProfilerItem_Count];
global_variable Profiling_Data global_writing_profiling_data[ProfilerItem_Count];
global_variable Profiling_Font global_profiling_font;

#define PROFILING_TIMED_BLOCK(ID) Profiling_Timed_Block ProfilingTimedBlock##ID(##ID)

#define BEGIN_PROFILING_TIMED_BLOCK(item) global_writing_profiling_data[item].begin_timer = platform_get_perf_counter(); \
global_writing_profiling_data[item].hit_count++

#define END_PROFILING_TIMED_BLOCK(item) global_writing_profiling_data[item].timer += platform_seconds_elapsed(global_writing_profiling_data[item].begin_timer) * 1000.f

struct Profiling_Timed_Block {
    
    Profiler_Item item;
    
    Profiling_Timed_Block(Profiler_Item item_init) {
        item = item_init;
        BEGIN_PROFILING_TIMED_BLOCK(item_init);
    }
    
    ~Profiling_Timed_Block() {
        END_PROFILING_TIMED_BLOCK(item);
    }
};