/* date = December 30th 2020 11:38 pm */

#define PLAYING_SOUND_LOOPING  0x01
#define PLAYING_SOUND_ACTIVE   0x02
#define PLAYING_SOUND_DEFAULT PLAYING_SOUND_LOOPING | PLAYING_SOUND_ACTIVE

struct Debug_Draw_Mixer {
    Loaded_Font playing_sound_font;
    Loaded_Font header_font;
};

struct Loaded_Sound {
    u32 num_channels;
    u32 num_samples;
    u32 samples_per_second;
    s16 *samples;
};

struct Playing_Sound {
    char *name;
    u32 flags;
    
    real32 volume;
    real32 target_volume;
    
    real32 fading_speed;
    real32 pan;
    
    Loaded_Sound *sound;
    u32 position;
};

struct Mixer {
    Playing_Sound playing_sounds[32];
    int next_playing_sound;
};

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
