internal Playing_Sound *
play_sound(Loaded_Sound *sound, b32 looping) {
    Playing_Sound *result = playing_sounds + next_playing_sound++;
    if (next_playing_sound >= array_count(playing_sounds)) {
        next_playing_sound = 0;
    }
    
    assert(sound && sound->samples);
    
    u32 flags = PLAYING_SOUND_DEFAULT;
    if (!looping) {
        flags &= ~PLAYING_SOUND_LOOPING;
    }
    result->flags = flags;
    result->sound = sound;
    result->position = 0;
    
    return result;
}

internal Loaded_Sound
load_sound(char *soundpath) {
    Loaded_Sound result = {};
    
    result = load_wav_file(soundpath);
    
    return result;
}