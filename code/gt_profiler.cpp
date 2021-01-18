#if PROFILER

#include <stdio.h> // For sprintf

global_variable Profiling_Data global_back_profiling_data[ProfilerItem_Count];
global_variable Profiling_Data global_writing_profiling_data[ProfilerItem_Count];
global_variable Profiling_Font global_profiling_font;

inline void
begin_profiling(Profiler_Item item) {
    global_writing_profiling_data[item].begin_timer = platform_get_perf_counter();
    global_writing_profiling_data[item].hit_count++;
}

inline void
end_profiling(Profiler_Item item) {
    global_writing_profiling_data[item].timer += platform_seconds_elapsed(global_writing_profiling_data[item].begin_timer) * 1000.f;
}

inline void
render_profiler(Vector2i dim, real32 dt) {
    glDisable(GL_DEPTH_TEST); // Prevent overlapping the profiler.
    
    Vector2 p = make_vector2(20.f, 40.f);
    Vector4 color = make_color(0xffaa55);
    
    
    Profiling_Data game_update_and_render_time = global_writing_profiling_data[ProfilerItem_GameUpdateAndRender];
    Profiling_Data game_audio_time = global_writing_profiling_data[ProfilerItem_Audio];
    
    char *vsync = global_vsync ? "ON" : "OFF";
    char *fps_lock = global_lock_fps ? "Locked" : "Unlocked";
    
    char buf[256];
    sprintf(buf, "Resolution: %d x %d\nFrame time: %.2fms\n%s\nUpdate & Render: %.2fms\nUpdate Audio: %.2fms\nVsync: %s\nFPS: %s\nDraw calls: %d\n", dim.width, dim.height, dt * 1000.f, open_gl->info.version, game_update_and_render_time.timer, game_audio_time.timer, vsync, fps_lock, draw_call_count);
    
    render_2d_right_handed(dim.width, dim.height);
    
    draw_text(p.x, p.y, (u8 *) buf, &global_profiling_font.font, color);
    
    glEnable(GL_DEPTH_TEST);
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