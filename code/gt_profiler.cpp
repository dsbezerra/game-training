#if PROFILER

#include <stdio.h>

global_variable profiling_data global_back_profiling_data[ProfilerItem_Count];
global_variable profiling_data global_writing_profiling_data[ProfilerItem_Count];
global_variable profiling_font global_profiling_font;

inline void
begin_profiling(profiler_item item) {
    global_writing_profiling_data[item].begin_timer = platform_get_perf_counter();
    global_writing_profiling_data[item].hit_count++;
}

inline void
end_profiling(profiler_item item) {
    global_writing_profiling_data[item].timer += platform_seconds_elapsed(global_writing_profiling_data[item].begin_timer) * 1000.f;
}

inline void
render_profiler(v2i dim, real32 dt) {
    v2 p = make_v2(20.f, 40.f);
    v4 color = make_color(0xffaa55);
    
    char buf[256];
    sprintf(buf, "Resolution: %d x %d\nFrame time: %.2fms", dim.width, dim.height, dt * 1000.f);
    
    render_right_handed(dim.width, dim.height);
    
    draw_text(p.x, p.y, (u8 *) buf, &global_profiling_font.font, color);
}

inline void
begin_profiler() {
    if (!global_profiling_font.loaded) {
        global_profiling_font.font = load_font("./data/fonts/Inconsolata-Regular.ttf", 16.f);
        global_profiling_font.loaded = true;
    }
    for (int i = 0; i < ProfilerItem_Count; i++) {
        global_back_profiling_data[i] = global_writing_profiling_data[i];
    }
    zero_array(global_writing_profiling_data);
}

#else

#define render_profiler(...)
#define begin_profiler(...)
#define begin_profiling(...)
#define end_profiling(...)

#endif