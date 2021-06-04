
global_variable Mixer mixer = {};

#define DRAW_DEBUG_MIXER 1

#define MIN_VOLUME 0.f
#define MAX_VOLUME 1.f

#if DRAW_DEBUG_MIXER
global_variable Debug_Draw_Mixer debug_draw_mixer = {};
#endif

internal void
set_volume(Playing_Sound *sound, real32 volume) {
    if (!sound) return;
    
    volume = clampf(MIN_VOLUME, volume, MAX_VOLUME);
    sound->volume = volume;
    sound->target_volume = volume;
};

internal void
fade_out(Playing_Sound *sound) {
    if (!sound) return;
    
    sound->target_volume = .0f;
};

internal void
fade_in(Playing_Sound *sound) {
    if (!sound) return;
    
    sound->target_volume = 1.0f;
};

internal void
set_flag(Playing_Sound *sound, u32 flag, b32 value) {
    if (!sound) return;
    
    if (value) {
        sound->flags |= flag;
    } else {
        sound->flags &= ~flag;
    }
}

internal void
set_active(Playing_Sound *sound, b32 active) {
    set_flag(sound, PLAYING_SOUND_ACTIVE, active);
}

internal void
set_looping(Playing_Sound *sound, b32 looping) {
    set_flag(sound, PLAYING_SOUND_LOOPING, looping);
}

internal void
stop_sound(Playing_Sound *sound) {
    if (!sound) return;
    set_active(sound, false);
    sound->position = 0;
}

internal void
pause_sound(Playing_Sound *sound) {
    if (!sound) return;
    set_active(sound, false);
}

internal void
resume_sound(Playing_Sound *sound) {
    if (!sound) return;
    set_active(sound, true);
}

internal void
restart_sound(Playing_Sound *sound) {
    stop_sound(sound);
    resume_sound(sound);
}

internal Playing_Sound *
play_sound(char *name, Loaded_Sound *sound, b32 looping) {
    Playing_Sound *result = mixer.playing_sounds + mixer.next_playing_sound++;
    if (mixer.next_playing_sound >= array_count(mixer.playing_sounds)) {
        mixer.next_playing_sound = 0;
    }
    
    assert(sound && sound->samples);
    
    u32 flags = PLAYING_SOUND_DEFAULT;
    
    result->name = name;
    result->flags = flags;
    result->sound = sound;
    result->position = 0;
    result->fading_speed = 3.f;
    result->volume = MAX_VOLUME;
    result->target_volume = MAX_VOLUME;
    
    set_flag(result, PLAYING_SOUND_LOOPING, looping);
    
    return result;
}

internal Loaded_Sound
load_sound(char *soundpath) {
    Loaded_Sound result = {};
    
    if (string_ends_with(soundpath, ".wav")) {
        result = load_wav_file(soundpath);
    } else if (string_ends_with(soundpath, ".ogg")) {
        result = load_ogg_file(soundpath);
    }
    
    return result;
}

internal Debug_Draw_Mixer
init_debug_draw_mixer() {
    Debug_Draw_Mixer result = {};
    
    result.playing_sound_font = load_font("./data/fonts/Inconsolata-Regular.ttf", 16.f);
    result.header_font = load_font("./data/fonts/Inconsolata-Bold.ttf", 18.f);
    
    return result;
}

#if DRAW_DEBUG_MIXER
internal void
draw_playing_sound(Vector2i dim, Playing_Sound *sound, real32 &y)
{
    real32 left_edge = dim.width * 0.02f;
    real32 bottom_edge = (real32) dim.height;
    
    real32 container_w  = dim.width;
    real32 container_h  = dim.height * 0.03f;
    
    real32 bar_w = container_w * .2f;
    real32 bar_h = container_h * .5f;
    
    Vector4 white              = make_color(0xffffffff);
    Vector4 container_color    = make_color(0xaa003f46);
    Vector4 bar_color          = make_color(0x20ffffff);
    Vector4 played_color       = make_color(0xffffffff);
    
    real32 container_top    = y - container_h;
    real32 container_bottom = y;
    real32 container_cy = (container_top + container_bottom) * .5f;
    
    //
    // Sound text
    //
    
    // Precalculate values
    char *name = sound->name ? sound->name : "Unnamed";
    real32 name_width = get_text_width(&debug_draw_mixer.playing_sound_font, name);
    
    
    s32 samples_per_channel = sound->sound->num_samples / sound->sound->num_channels;
    s32 played_samples_per_channel = sound->position / sound->sound->num_channels;
    
    assert(sound->sound->samples_per_second == 44100);
    
    s32 seconds_elapsed = played_samples_per_channel / sound->sound->samples_per_second;
    s32 total_sound_in_seconds = samples_per_channel / sound->sound->samples_per_second;
    
    s32 played_minutes = seconds_elapsed / 60;
    s32 played_seconds = seconds_elapsed % 60;
    
    s32 total_minutes = total_sound_in_seconds / 60;
    s32 total_seconds = total_sound_in_seconds % 60;
    
    char played_time[32];
    sprintf(played_time, "  -  %02d:%02d ", played_minutes, played_seconds);
    real32 played_text_width = get_text_width(&debug_draw_mixer.playing_sound_font, played_time);
    
    char total_time[32];
    sprintf(total_time, " %02d:%02d | ", total_minutes, total_seconds);
    real32 total_text_width = get_text_width(&debug_draw_mixer.playing_sound_font, total_time);
    
    real32 text_cy = container_cy + debug_draw_mixer.playing_sound_font.line_height * .34f;
    real32 text_cx = container_w * .01f;
    
    immediate_begin();
    
    //
    // Container
    //
    
    {
        Vector2 min = make_vector2(.0f, container_top);
        Vector2 max = make_vector2(container_w, container_bottom);
        immediate_quad(min, max, -1.f, container_color);
    }
    
    real32 bar_padding = bar_h * .5f;
    
    real32 bar_left = text_cx + name_width + bar_padding + played_text_width;
    real32 bar_right = bar_left + bar_w;
    real32 bar_top    = container_top    + bar_padding;
    real32 bar_bottom = container_bottom - bar_padding;
    
    //
    // Bar color
    //
#if 1
    {
        
        Vector2 min = make_vector2(bar_left,  bar_top);
        Vector2 max = make_vector2(bar_right, bar_bottom);
        immediate_quad(min, max, -1.f, bar_color);
    }
    
    //
    // Played color
    //
    {
        real32 played_ratio = (real32) sound->position / (real32) sound->sound->num_samples;
        Vector2 min = make_vector2(bar_left,                   bar_top);
        Vector2 max = make_vector2(min.x + bar_w*played_ratio, bar_bottom);
        immediate_quad(min, max, -1.f, played_color);
    }
#else
    u32 samples_per_pixel = 128;
    u32 bytes_per_sample = 24 / 8 * sound->sound->num_channels;
    
    u32 position = 0;
    
    real32 sample_width = bar_w / samples_per_pixel;
    for (real32 x = bar_left;
         x < bar_right;
         x += sample_width)
    {
        s16 low = 0;
        u32 sample_position = 0;
        s16 high = 0;
        
        for (u32 sample_index = position;
             sample_index < position + samples_per_pixel;
             sample_index += 2)
        {
            u32 index = sample_index * samples_per_pixel * bytes_per_sample;
            if (index >= sound->sound->num_samples || index == 0) break;
            
            sample_position = index;
            
            s16 sample = sound->sound->samples[index];
            if (sample < low)  low  = sample;
            if (sample > high) high = sample;
        }
        
        position += samples_per_pixel;
        
        real32 l_percent = ((real32)low  - MIN_S16) / MAX_U16;
        real32 h_percent = ((real32)high - MIN_S16) / MAX_U16;
        
        real32 low_height  = bar_h * l_percent;
        real32 high_height = bar_h * h_percent;
        
        Vector4 color = bar_color;
        if (sample_position < sound->position) {
            color = played_color;
        }
        
        real32 half_sample_width = sample_width *.5f;
        real32 bar_center = bar_bottom - bar_h *.5f;
        Vector2 min = make_vector2(x - half_sample_width, bar_center - high_height);
        Vector2 max = make_vector2(x + half_sample_width, bar_center + low_height);
        immediate_quad(min, max, -1.f, color);
    }
#endif
    immediate_flush();
    
    // Draw name
    draw_text(text_cx, text_cy, (u8 *) name, &debug_draw_mixer.playing_sound_font, white);
    
    //
    // Draw timestamps
    //
    
    // Draw played timestamp
    {
        real32 played_timestamp_x = bar_left - played_text_width - bar_padding;
        draw_text(played_timestamp_x, text_cy, (u8 *) played_time, &debug_draw_mixer.playing_sound_font, white);
    }
    
    // Draw total timestamp
    {
        real32 total_timestamp_x = bar_right + bar_padding;
        draw_text(total_timestamp_x, text_cy, (u8 *) total_time, &debug_draw_mixer.playing_sound_font, white);
    }
    
    b32 looping = sound->flags & PLAYING_SOUND_LOOPING;
    char buf[256];
    sprintf(buf, "Looping: %s | Pan: %.2f | Volume: %.2f", looping ? "true" : "false", sound->pan, sound->volume);
    
    real32 rest_of_stuff_x = bar_right + bar_padding + total_text_width;
    draw_text(rest_of_stuff_x, text_cy, (u8 *) buf, &debug_draw_mixer.playing_sound_font, white);
    y -= container_h;
}
#endif

internal void
draw_debug_draw_mixer(Vector2i dimensions) {
#if DRAW_DEBUG_MIXER
    if (!debug_draw_mixer.header_font.texture) {
        debug_draw_mixer = init_debug_draw_mixer();
    }
    
    render_2d_right_handed(dimensions.width, dimensions.height);
    
    real32 x = dimensions.width * 0.002f;
    real32 y = (real32) dimensions.height;
    
    u32 count = 0;
    
    for (Playing_Sound *sound = mixer.playing_sounds; sound != mixer.playing_sounds + array_count(mixer.playing_sounds); sound++) {
        if (!(sound->flags & PLAYING_SOUND_ACTIVE)) continue;
        if (sound->sound->num_samples == 0) continue;
        
        count++;
        draw_playing_sound(dimensions, sound, y);
    }
    
    if (count > 0) {
        real32 height = dimensions.height * 0.03f;
        {
            Vector4 color = make_color(0xbb003f46);
            Vector2 min = make_vector2(.0f, y - height);
            Vector2 max = make_vector2(dimensions.width, y);
            
            immediate_begin();
            immediate_quad(min, max, -1.f, color);
            immediate_flush();
        }
        y -= height*.3f;
        
        char buf[256];
        sprintf(buf, "Playing sounds: %d", count);
        u8 *title = (u8*) buf;
        draw_text(x, y, title, &debug_draw_mixer.header_font, make_color(0xffffffff));
    }
#endif
}