#if PROFILER

#include <stdio.h> // For sprintf

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
    
    opengl_memory_info memory_info = opengl_get_memory_info();
    
    GLint usage = memory_info.total_memory_in_kb - memory_info.current_available_memory_in_kb;
    
    profiling_data game_update_and_render_time = global_writing_profiling_data[ProfilerItem_GameUpdateAndRender];
    
    char *vsync = global_vsync ? "ON" : "OFF";
    char *fps_lock = global_lock_fps ? "Locked" : "Unlocked";
    
    char buf[256];
    sprintf(buf, "Resolution: %d x %d\nFrame time: %.2fms\nGPU Memory: using %dMB of %dMB\n%s\nUpdate Render: %.2fms\nVsync: %s\nFPS: %s\nDraw calls: %d\n", dim.width, dim.height, dt * 1000.f, usage / 1024, memory_info.total_memory_in_kb / 1024, open_gl->info.version, game_update_and_render_time.timer, vsync, fps_lock, draw_call_count);
    
    render_2d_right_handed(dim.width, dim.height);
    
    draw_text(p.x, p.y, (u8 *) buf, &global_profiling_font.font, color);
}

inline void
begin_profiler() {
    if (!global_profiling_font.loaded) {
        global_profiling_font.font = load_font("./data/fonts/Inconsolata-Bold.ttf", 16.f);
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