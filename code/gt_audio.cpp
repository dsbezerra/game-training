
global_variable Mixer mixer = {};

#define DRAW_DEBUG_MIXER 1

#if DRAW_DEBUG_MIXER
global_variable Debug_Draw_Mixer debug_draw_mixer = {};
#endif

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
    result->volume = 1.f;
    
    set_flag(result, PLAYING_SOUND_LOOPING, looping);
    
    return result;
}

internal Loaded_Sound
load_sound(char *soundpath) {
    Loaded_Sound result = {};
    
    result = load_wav_file(soundpath);
    
    return result;
}

internal Debug_Draw_Mixer
init_debug_draw_mixer() {
    Debug_Draw_Mixer result = {};
    
    result.playing_sound_font = load_font("./data/fonts/Inconsolata-Regular.ttf", 14.f);
    result.header_font = load_font("./data/fonts/Inconsolata-Bold.ttf", 16.f);
    
    return result;
}

internal void
draw_debug_draw_mixer(Vector2i dimensions) {
#if DRAW_DEBUG_MIXER
    if (!debug_draw_mixer.header_font.texture) {
        debug_draw_mixer = init_debug_draw_mixer();
    }
    
    real32 x = dimensions.width * 0.002f;
    real32 y = (real32) dimensions.height;
    
    real32 height = dimensions.height * 0.04f;
    
    Vector4 played_color = make_color(0xff00d5ed);
    Vector4 bar_color    = make_color(0xff003f46);
    Vector4 white        = make_color(0xffffffff);
    Vector4 shadow       = make_color(0xff000000);
    
    u32 count = 0;
    
    for (Playing_Sound *sound = mixer.playing_sounds; sound != mixer.playing_sounds + array_count(mixer.playing_sounds); sound++) {
        if (!(sound->flags & PLAYING_SOUND_ACTIVE)) continue;
        if (sound->sound->num_samples == 0) continue;
        
        count++;
        
        immediate_begin();
        
        //
        // Bar color
        //
        {
            Vector2 min = make_vector2(.0f, y);
            Vector2 max = make_vector2((real32) dimensions.width, y - height);
            immediate_quad(min, max, bar_color);
        }
        
        //
        // Played color
        //
        {
            real32 played_ratio = (real32) sound->position / (real32) sound->sound->num_samples;
            Vector2 min = make_vector2(.0f, y);
            Vector2 max = make_vector2((real32) dimensions.width * played_ratio, y - height);
            immediate_quad(min, max, played_color);
        }
        
        immediate_flush();
        
        //
        // Draw info about playing sound
        //
        {
            char buf[512];
            
            b32 looping = sound->flags & PLAYING_SOUND_LOOPING;
            sprintf(buf, "%s - Looping: %s\n", sound->name ? sound->name : "Unnamed", looping ? "true" : "false");
            
            real32 cy = y - height * .5f + debug_draw_mixer.playing_sound_font.line_height * .4f;
            real32 tx = x + 2.f;
            draw_text(tx + 1.f, cy + 1.f, (u8 *) buf, &debug_draw_mixer.playing_sound_font, shadow);
            draw_text(tx, cy, (u8 *) buf, &debug_draw_mixer.playing_sound_font, white);
        }
        
        y -= height;
    }
    
    if (count > 0) {
        y -= height * .5f;
        
        char buf[256];
        sprintf(buf, "Playing sounds: %d", count);
        u8 *title = (u8*) buf;
        draw_text(x, y + 1.f, title, &debug_draw_mixer.header_font, shadow);
        draw_text(x, y, title, &debug_draw_mixer.header_font, white);
    }
#endif
}