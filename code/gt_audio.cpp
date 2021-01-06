global_variable Mixer mixer = {};

internal void
set_volume(Playing_Sound *sound, real32 volume) {
    if (!sound) return;
    // @Cleanup: make .0f and 1.f global constants
    sound->volume = clampf(0.f, volume, 1.f);
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

internal Playing_Sound *
play_sound(Loaded_Sound *sound, b32 looping) {
    Playing_Sound *result = mixer.playing_sounds + mixer.next_playing_sound++;
    if (mixer.next_playing_sound >= array_count(mixer.playing_sounds)) {
        mixer.next_playing_sound = 0;
    }
    
    assert(sound && sound->samples);
    
    u32 flags = PLAYING_SOUND_DEFAULT;
    set_flag(result, PLAYING_SOUND_LOOPING, looping);
    
    result->flags = flags;
    result->sound = sound;
    result->position = 0;
    result->volume = 1.f;
    
    return result;
}

internal Loaded_Sound
load_sound(char *soundpath) {
    Loaded_Sound result = {};
    
    result = load_wav_file(soundpath);
    
    return result;
}