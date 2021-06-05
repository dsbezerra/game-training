/* date = December 30th 2020 11:38 pm */

#define PLAYING_SOUND_LOOPING  0x01
#define PLAYING_SOUND_ACTIVE   0x02
#define PLAYING_SOUND_DEFAULT PLAYING_SOUND_LOOPING | PLAYING_SOUND_ACTIVE

#define DEBUG_DRAW_MIXER_DEFAULT  0x0
#define DEBUG_DRAW_MIXER_WAVEFORM 0x1

struct Debug_Draw_Mixer {
    u32 flags;
    Loaded_Font playing_sound_font;
    Loaded_Font header_font;
};

struct Loaded_Sound {
    u32 num_channels;
    u32 num_samples;
    u32 samples_per_second;
    s16 *samples;
};

struct Debug_Waveform_Sample {
    real32 x;
    
    s16 low;
    s16 high;
    
    u32 low_offset;
    u32 high_offset;
    
    real32 low_percent;
    real32 high_percent;
};

struct Debug_Waveform {
    b32 ready;
    
    real32 left;
    real32 top;
    
    real32 width;
    real32 height;
    real32 center;
    
    Debug_Waveform_Sample *samples;
    u32 num_samples;
    
    b32 is_mouse_over;
};

struct Playing_Sound {
    char *name;
    u32 flags;
    
    real32 volume;
    real32 target_volume;
    
    real32 fading_speed;
    real32 pan;
    
    Loaded_Sound *sound;
    Debug_Waveform waveform;
    u32 position;
};

struct Mixer {
    Playing_Sound playing_sounds[32];
    int next_playing_sound;
};

internal Debug_Waveform
make_waveform(real32 start_x, real32 start_y, real32 width, real32 height, real32 scale, Loaded_Sound *sound) {
    Debug_Waveform result = {};
    result.ready = true;
    
    u32 width_u32 = (u32) width;
    u32 samples_per_pixel = (sound->num_samples / sound->num_channels) / width_u32;
    
    real32 top = start_y;
    real32 bottom = top + height;
    
    real32 left = start_x;
    real32 right = left + width;
    real32 center = (top + bottom) * .5f;
    
    result.left   = left;
    result.top    = top;
    result.width  = width;
    result.height = height;
    result.center = center;
    
    result.num_samples = right - left;
    result.samples = (Debug_Waveform_Sample *) platform_alloc(sizeof(Debug_Waveform_Sample) * result.num_samples);
    
    s16 *samples = sound->samples;
    Debug_Waveform_Sample *at = result.samples;
    
    for (real32 x = 0.f; x < width-1; ++x)
    {
        s16 low  = 0;
        s16 high = 0;
        
        u32 position = x * samples_per_pixel*sound->num_channels;
        for (u32 s = 0; s < samples_per_pixel; s += 2) {
            u32 min_offset = position + s * sound->num_channels;
            u32 max_offset = position + s * sound->num_channels + 1;
            s16 min_sample = samples[min_offset];
            s16 max_sample = samples[max_offset];
            if (min_sample < low ) low  = min_sample;
            if (max_sample > high) high = max_sample;
        }
        
        real32 l_percent = ((real32)low   - MIN_S16) / MAX_U16;
        real32 h_percent = ((real32)high  - MIN_S16) / MAX_U16;
        
        l_percent *= scale;
        h_percent *= scale;
        
        Debug_Waveform_Sample item = {};
        item.x = left + x;
        item.low = low;
        item.high = high;
        item.low_offset = position;
        item.high_offset = position + samples_per_pixel;
        item.low_percent = l_percent;
        item.high_percent = h_percent;
        *at++ = item;
    }
    
    return result;
}

internal void set_volume(Playing_Sound *sound, real32 volume);
internal void fade_in(Playing_Sound *sound);
internal void fade_out(Playing_Sound *sound);
internal void set_flag(Playing_Sound *sound, u32 flag, b32 value);
internal void set_active(Playing_Sound *sound, b32 active);
internal void set_looping(Playing_Sound *sound, b32 looping);
internal void stop_sound(Playing_Sound *sound);
internal void pause_sound(Playing_Sound *sound);
internal void resume_sound(Playing_Sound *sound);
internal void restart_sound(Playing_Sound *sound);
internal Playing_Sound * play_sound(char *name, Loaded_Sound *sound, b32 looping = true);
internal Loaded_Sound load_sound(char *soundpath);

internal Debug_Draw_Mixer init_debug_draw_mixer();

internal void draw_debug_draw_mixer(Vector2i dimensions);

