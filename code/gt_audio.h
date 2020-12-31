/* date = December 30th 2020 11:38 pm */

struct Loaded_Sound {
    u32 num_channels;
    u32 num_samples;
    s16*samples;
};

struct Playing_Sound {
    Loaded_Sound *sound;
    u32 position;
    b32 looping;
    b32 active;
};

Playing_Sound playing_sounds[32];
int next_playing_sound = 0;

internal Playing_Sound *
play_sound(Loaded_Sound *sound, b32 looping) {
    Playing_Sound *result = playing_sounds + next_playing_sound++;
    if (next_playing_sound >= array_count(playing_sounds)) {
        next_playing_sound = 0;
    }
    
    result->sound = sound;
    result->active = true;
    result->position = 0;
    result->looping = looping;
    
    return result;
}

internal Loaded_Sound load_sound(char *soundpath);
