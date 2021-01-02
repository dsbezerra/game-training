global_variable Mixer mixer = {};

internal void
set_active(Playing_Sound *sound, b32 active) {
    if (!sound) return;
    if (active) {
        sound->flags |= PLAYING_SOUND_ACTIVE;
    } else {
        sound->flags &= ~PLAYING_SOUND_ACTIVE;
    }
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