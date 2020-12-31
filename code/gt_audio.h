/* date = December 30th 2020 11:38 pm */

#define PLAYING_SOUND_LOOPING 0x01
#define PLAYING_SOUND_ACTIVE  0x02
#define PLAYING_SOUND_DEFAULT PLAYING_SOUND_LOOPING | PLAYING_SOUND_ACTIVE

struct Loaded_Sound {
    u32 num_channels;
    u32 num_samples;
    s16*samples;
};

struct Playing_Sound {
    u32 flags;
    
    Loaded_Sound *sound;
    u32 position;
};

Playing_Sound playing_sounds[32];
int next_playing_sound = 0;

//
//
//

internal Playing_Sound * play_sound(Loaded_Sound *sound, b32 looping = true);
internal Loaded_Sound load_sound(char *soundpath);
