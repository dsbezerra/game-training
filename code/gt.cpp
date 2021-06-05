#include "gt.h"
#include "gt_memory.cpp"
#include "gt_serialization.cpp"
#include "gt_hotloader.cpp"
#include "gt_font.cpp"
#include "gt_opengl.cpp"
#include "gt_shader.cpp"
#include "gt_draw.cpp"
#include "gt_ogg_loader.cpp"
#include "gt_audio.cpp"
#include "gt_profiler.cpp"
#include "gt_mesh.cpp"
#include "gt_sprite.cpp"

global_variable Game_Core core = {};
global_variable Debug_Draw debug_draw = {};

#include "gt_camera.cpp"

global_variable Loaded_Font menu_title_font;
global_variable Loaded_Font menu_item_font;

global_variable real32 global_press_t = .0f;

internal void
game_frame_begin(int width, int height) {
    ensure_framebuffer(width, height);
    
    if (use_multisampling) {
        use_framebuffer(&immediate->multisampled_framebuffer);
    }
    
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    render_2d_right_handed(width, height);
}

internal void
draw_menu(char *game_title, Game_Memory *memory) {
    
    Vector2i dim = memory->window_dimensions;
    
    use_framebuffer(&immediate->menu_framebuffer);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    render_2d_right_handed(dim.width, dim.height);
    
    Vector4 white          = make_color(0xffffffff);
    Vector4 default_color  = make_color(0xffaaaaaa);
    Vector4 selected_color = make_color(0xffffff00);
    
    char *menu_title = memory->game_mode == GameMode_GameOver ? "Game Over" : game_title;
    real32 menu_title_width = get_text_width(&menu_title_font, menu_title);
    
    real32 y = dim.height * 0.2f;
    
    //
    // Title
    //
    //  Retry
    //  Quit
    //
    
    draw_text((dim.width - menu_title_width) / 2.f, y, (u8 *) menu_title, &menu_title_font, white);
    
    char* menu_items[] = {"Retry", memory->quit_was_selected ? "Quit? Are you sure?" : "Quit"};
    
    y += dim.height * 0.25f;
    for (int menu_item = 0; menu_item < array_count(menu_items); ++menu_item) {
        
        char *text = menu_items[menu_item];
        real32 width = get_text_width(&menu_item_font, text);
        
        if (memory->menu_selected_item == menu_item) {
            Vector4 non_white = make_color(0xffffde00);
            
            real32 now = cosf(core.time_info.current_time);
            real32 t = cosf(now * 3);
            t *= t;
            
            t = .4f + .54f * t;
            Vector4 front_color = lerp_color(non_white, t, white);
            
            // Also draw an extra background item.
            real32 offset = menu_item_font.line_height / 40;
            draw_text((dim.width - width) / 2.f, y, (u8 *) text, &menu_item_font, selected_color);
            draw_text((dim.width - width) / 2.f + offset, y - offset, (u8 *) text, &menu_item_font, front_color);
        } else {
            draw_text((dim.width - width) / 2.f, y, (u8 *) text, &menu_item_font, default_color);
        }
        
        y += (real32) menu_item_font.line_height;
    }
}

inline GLint
load_texture(char *filename) {
    u32 texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(true);
    // load and generate the texture
    int width, height, nrChannels;
    u8 *data = stbi_load(filename, &width, &height, &nrChannels, 4);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        open_gl->glGenerateMipmap(GL_TEXTURE_2D);
    }
    else { invalid_code_path; }
    
    stbi_image_free(data);
    
    return texture;
}

internal void
load_menu_arts(App_State *state) {
    state->menu_art.dodger          = load_texture("./data/menu_arts/dodger.png");
    state->menu_art.memory_puzzle   = load_texture("./data/menu_arts/memory_puzzle.png");
    state->menu_art.slide_puzzle    = load_texture("./data/menu_arts/slide_puzzle.png");
    state->menu_art.simon           = load_texture("./data/menu_arts/simon.png");
    state->menu_art.nibbles         = load_texture("./data/menu_arts/nibbles.png");
    state->menu_art.tetris          = load_texture("./data/menu_arts/tetris.png");
    state->menu_art.katamari        = load_texture("./data/menu_arts/katamari.png");
    state->menu_art.sokoban         = load_texture("./data/menu_arts/sokoban.png");
    state->menu_art.othello         = load_texture("./data/menu_arts/othello.png");
    state->menu_art.flood_it        = load_texture("./data/menu_arts/flood_it.png");
}

internal real32
get_time() {
    return platform_seconds_elapsed(core.time_info.start_counter);
}

internal b32
is_mouse_over(Vector2 position, real32 width, real32 height) {
    b32 result = false;
    
    Vector2i mouse_position;
    platform_get_cursor_position(&mouse_position);
    
    real32 max_x = position.x + width;
    real32 max_y = position.y + height;
    
    if (mouse_position.x < position.x || mouse_position.x > max_x) return result;
    if (mouse_position.y < position.y || mouse_position.y > max_y) return result;
    
    result = true;
    
    return result;
}

internal void
free_menu_arts(App_State *state) {
    const GLuint arts[] = {
        state->menu_art.dodger,
        state->menu_art.memory_puzzle,
        state->menu_art.slide_puzzle,
        state->menu_art.simon,
        state->menu_art.nibbles,
        state->menu_art.tetris,
        state->menu_art.katamari,
        state->menu_art.sokoban,
        state->menu_art.othello,
        state->menu_art.flood_it,
    };
    glDeleteTextures(array_count(arts), arts);
}

#include "games/dodger.cpp"
#include "games/memory_puzzle.cpp"
#include "games/slide_puzzle.cpp"
#include "games/simon.cpp"
#include "games/nibbles.cpp"
#include "games/tetris.cpp"
#include "games/katamari.cpp"
#include "games/sokoban.cpp"
#include "games/othello.cpp"
#include "games/flood_it.cpp"
#include "games/connect_four.cpp"
#include "games/bejeweled.cpp"

//
// Game titles
//
global_variable char* game_titles[] = {
    "",
    DODGER_TITLE,
    MEMORY_PUZZLE_TITLE,
    SLIDE_PUZZLE_TITLE,
    SIMON_TITLE,
    NIBBLES_TITLE,
    TETRIS_TITLE,
    KATAMARI_DAMACY_TITLE,
    SOKOBAN_TITLE,
    OTHELLO_TITLE,
    FLOOD_IT_TITLE,
    CONNECT_FOUR_TITLE,
    BEJEWELED_TITLE,
};

//
// Game menu art table
//
void stub_menu_art(App_State *state, Vector2 min, Vector2 max) { assert(false); }
void (*menu_table[])(App_State *state, Vector2 min, Vector2 max) = {
    stub_menu_art,
    dodger_menu_art,
    memory_puzzle_menu_art,
    slide_puzzle_menu_art,
    simon_menu_art,
    nibbles_menu_art,
    tetris_menu_art,
    katamari_menu_art,
    sokoban_menu_art,
    othello_menu_art,
    flood_it_menu_art,
    connect_four_menu_art,
    bejeweled_menu_art,
};

//
// Game custom free table
// 
void stub_game_free(Game_Memory *memory) { assert(false); }
void (*game_free_table[])(Game_Memory *memory) = {
    0, // None
    0, // Dodger
    0, // Memory Puzzle 
    0, // Slide Puzzle
    0, // Simon
    0, // Nibbles
    0, // Tetris
    katamari_game_free,
    sokoban_game_free,
    othello_game_free,
    flood_it_game_free,
    connect_four_game_free,
    bejeweled_game_free,
};

// 
// Game update and render table
//
void stub_game_update_and_render(Game_Memory *memory, Game_Input *input) { assert(false); }
void (*game_table[])(Game_Memory *memory, Game_Input *input) = {
    stub_game_update_and_render,
    dodger_game_update_and_render,
    memory_puzzle_game_update_and_render,
    slide_puzzle_game_update_and_render,
    simon_game_update_and_render,
    nibbles_game_update_and_render,
    tetris_game_update_and_render,
    katamari_game_update_and_render,
    sokoban_game_update_and_render,
    othello_game_update_and_render,
    flood_it_game_update_and_render,
    connect_four_game_update_and_render,
    bejeweled_game_update_and_render,
};

internal void *
game_alloc(Game_Memory *memory, u64 size) {
    memory->permanent_storage_size = size;
    memory->permanent_storage = platform_alloc(size);
    memory->initialized = true;
    return memory->permanent_storage;
}

internal void
game_free(Game_Memory *memory, game current_game) {
    if (!memory) {
        return;
    }
    if (memory->permanent_storage) {
        if (game_free_table[current_game]) {
            game_free_table[current_game](memory);
        }
        platform_free(memory->permanent_storage);
    }
    memory->permanent_storage = 0;
    memory->permanent_storage_size = 0;
    memory->initialized = false;
    memory->asked_to_quit = false;
    memory->quit_was_selected = false;
}

internal void
game_quit(App_State *state, Game_Memory *memory) {
    state->current_selecting_game = state->current_game;
    state->current_mode = Mode_SelectingGame;
    load_menu_arts(state);
    game_free(memory, state->current_game);
}

internal void
advance_menu_choice(s8 *current_choice, s8 delta) {
    *current_choice += delta;
    
    if (*current_choice < 0) *current_choice += (s8) GameMenuItem_Count;
    if (*current_choice >= (s8) GameMenuItem_Count) *current_choice -= (s8) GameMenuItem_Count;
}

internal void
advance_game(App_State *state, int value) {
    int new_game = ((int) state->current_selecting_game) + value;
    if (new_game < Game_None) {
        new_game = (int) (Game_Count - 1);
    } else if (new_game >= Game_Count) {
        new_game = (int) Game_None;
    }
    state->current_selecting_game = (game) new_game;
    global_press_t = .0f;
}

internal void
game_output_sound(Game_Sound_Buffer *sound_buffer, Game_Input *input) {
    
    s16 *at = sound_buffer->samples;
    for (int sample_index = 0; sample_index < sound_buffer->samples_to_write; ++sample_index) {
        
        s16 left_sample = 0;
        s16 right_sample = 0;
        
        for (Playing_Sound *sound = mixer.playing_sounds; sound != mixer.playing_sounds + array_count(mixer.playing_sounds); sound++) {
            if (!(sound->flags & PLAYING_SOUND_ACTIVE)) continue;
            
            sound->volume = move_towards(sound->volume, sound->target_volume, sound->fading_speed/sound_buffer->samples_per_second);
            
            if (sound->sound && sound->sound->samples && sound->volume) {
                
                s16 l = sound->sound->samples[sound->position++];
                s16 r = sound->sound->samples[sound->position++];
                
                left_sample   += (s16) (l * clampf(0.f, 1.f - sound->pan, 1.f) * sound->volume);
                right_sample  += (s16) (r * clampf(0.f, 1.f + sound->pan, 1.f) * sound->volume);
                
                if (sound->position >= sound->sound->num_samples) {
                    if (sound->flags & PLAYING_SOUND_LOOPING) {
                        sound->position -= sound->sound->num_samples;
                    } else {
                        set_active(sound, false);
                    }
                }
            }
        }
        
        *at++ = left_sample;
        *at++ = right_sample;
    }
}

internal void
update_mode_selecting(App_State *state, Game_Input *input) {
    
    real32 press_t_target = 0.2f;
    
    if (pressed(Button_Left)) {
        advance_game(state, -1);
    } else if (is_down(Button_Left)) {
        if (global_press_t >= press_t_target) {
            advance_game(state, -1);
        }
        global_press_t += core.time_info.dt;
    }
    if (pressed(Button_Right)) {
        advance_game(state, 1);
    } else if (is_down(Button_Right)) {
        if (global_press_t >= press_t_target) {
            advance_game(state, 1);
        }
        global_press_t += core.time_info.dt;
    }
    if (pressed(Button_Enter)) {
        if (state->current_selecting_game > Game_None && state->current_selecting_game < Game_Count) {
            if (state->current_selecting_game != state->current_game) {
                state->current_game = state->current_selecting_game;
                state->current_selecting_game = Game_None;
                game_free(state->memory, state->current_game);
            }
            free_menu_arts(state);
            state->current_mode = Mode_PlayingGame;
        }
    }
    
    if (pressed(Button_Escape) && state->current_game != Game_None) {
        state->current_mode = Mode_PlayingGame;
    }
}

internal void
render_mode_selecting(App_State *state) {
    
    GLsizei width = (GLsizei) state->window_dimensions.x;
    GLsizei height = (GLsizei) state->window_dimensions.y;
    
    use_framebuffer((GLuint) 0);
    
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width, height);
    
    render_2d_right_handed(width, height);
    
    real32 selection_width = max(250.f, state->window_dimensions.width / 4.f);
    real32 selection_height = selection_width * 16.f / 9.f;
    
    real32 remaining = state->window_dimensions.height - selection_height;
    
    real32 border_width = selection_width * 0.012f;
    
    real32 now = core.time_info.current_time;
    real32 t = cosf(now);
    t *= t;
    t = .4f + t;
    
    Vector4 border_color = make_vector4(1.f, 1.0f, 1.f, .4f);
    Vector4 border_color_almost_transparent = make_vector4(1.f, 1.0f, 1.f, .2f);
    
    real32 offset = (real32) (selection_width * (int) state->current_selecting_game);
    Mat4 translation = translate(make_vector2(-offset + state->window_dimensions.width * .5f, -remaining * .5f));
    
#if 0
    real32 tCos = cosf(time_info.current_time);
    tCos *= tCos;
    tCos = .95f + .05f * tCos;
    Mat4 center = translate(make_vector2(state->window_dimensions.width * 0.5f - offset, .0f));
    view_matrix = view_matrix * center * scale(make_vector2(tCos, tCos));
#else
    view_matrix = view_matrix * translation;
#endif
    
    refresh_shader_transform();
    
    for (int index = 1; index < array_count(game_titles); ++index) {
        real32 x = (real32) index * selection_width;
        Vector2 min  = make_vector2(x, 0.f);
        Vector2 max = make_vector2(x + selection_width, selection_height);
        
        if (index < array_count(menu_table)) {
            menu_table[index](state, min, max);
        }
        if (index == (int) state->current_selecting_game) {
            Vector2 top_min = make_vector2(min.x, min.y);
            Vector2 top_max = make_vector2(max.x, min.y + border_width);
            
            Vector2 bottom_min = make_vector2(min.x, max.y - border_width);
            Vector2 bottom_max = make_vector2(max.x, max.y);
            
            Vector2 left_min = make_vector2(min.x, top_max.y);
            Vector2 left_max = make_vector2(min.x + border_width, bottom_min.y);
            
            Vector2 right_min = make_vector2(max.x - border_width, top_max.y);
            Vector2 right_max = make_vector2(max.x, bottom_min.y);
            
            immediate_begin();
            
            // Draw select quad
            
            Vector4 a = make_vector4(1.f, 1.0f, 1.f, .2f);
            Vector4 b = make_vector4(1.f, 1.0f, 1.f, .0f);
            immediate_quad(min, max, lerp_color(b, t, a));
            
            // Draw select borders
            Vector4 border_t_color = lerp_color(border_color, t, border_color_almost_transparent);
            immediate_quad(left_min, left_max, border_t_color);
            immediate_quad(right_min, right_max, border_t_color);
            immediate_quad(top_min, top_max, border_t_color);
            immediate_quad(bottom_min, bottom_max, border_t_color);
            
            immediate_flush();
        }
        
        char* title = game_titles[index];
        real32 size = get_text_width(&state->game_title_font, title);
        
        Vector4 text_color = make_color(0xffffffff);
        Vector4 backing_color = make_color(0xff111111);
        
        real32 text_x = (min.x + max.x - size) * .5f;
        real32 text_y = max.height * 0.2f; 
        
        draw_text(text_x + 2.f, text_y + 2.f, (u8 *) title, &state->game_title_font, backing_color);
        draw_text(text_x, text_y, (u8 *) title, &state->game_title_font, text_color);
    }
}

internal void
draw_framebuffer(Vector2i dim) {
    
    if (immediate->framebuffer_to_blit) {
        Opengl_Framebuffer screen       = immediate->screen_framebuffer;
        open_gl->glBindFramebuffer(GL_READ_FRAMEBUFFER, immediate->framebuffer_to_blit->framebuffer_handle);
        open_gl->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, screen.framebuffer_handle);
        open_gl->glBlitFramebuffer(0, 0, dim.width, dim.height, 0, 0, dim.width, dim.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        
        use_framebuffer((GLuint) 0);
        
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        set_shader(global_screen_shader);
        
        Vector2 min = make_vector2(-1.f, -1.0f);
        Vector2 max = make_vector2(1.f, 1.f);
        
        immediate_begin();
        immediate_textured_quad(min, max, screen.color_handle);
        immediate_flush();
        
        if (debug_draw.draw_shadow_map && immediate->depth_map.id) {
            set_shader(global_depth_shader);
            immediate_begin();
            Vector2 min_shadow_map = make_vector2(-1.f, -1.f);
            Vector2 max_shadow_map = make_vector2(1.f,  1.f);
            immediate_textured_quad(min_shadow_map, max_shadow_map, immediate->depth_map.id);
            immediate_flush();
        }
    } else {
        OutputDebugString("No framebuffer to blit\n");
    }
}

internal void
update_debug(Game_Input *input) {
    if (input->alt_is_down) {
        if (pressed(Button_F1)) {
            debug_draw.draw_shadow_map = !debug_draw.draw_shadow_map;
        }
        if (pressed(Button_F2)) {
            debug_draw.draw_mixer = !debug_draw.draw_mixer;
        }
        
#define SEEK_STEP 44100 // 1 sec per press
        
        if (debug_draw.draw_mixer) {
            s32 seek_amount = 0;
            if (is_down(Button_Left)) {
                seek_amount = -SEEK_STEP;
            }
            if (is_down(Button_Right)) {
                seek_amount = SEEK_STEP;
            }
            // NOTE(diego): Seek all playing sounds for now just for testing.
            if (seek_amount != 0) {
                for (Playing_Sound *sound = mixer.playing_sounds; sound != mixer.playing_sounds + array_count(mixer.playing_sounds); sound++) {
                    if (!(sound->flags & PLAYING_SOUND_ACTIVE)) continue;
                    if (sound->sound->num_samples == 0) continue;
                    
                    sound->position = clamp(0, sound->position + seek_amount, sound->sound->num_samples);
                }
            }
        }
    }
}

internal void
game_update_and_render(App_State *state, Game_Memory *memory, Game_Input *input) {
    if (!state->initialized) {
        state->current_mode = Mode_SelectingGame;
        state->current_game = Game_None;
        state->game_title_font = load_font("./data/fonts/Inconsolata-Bold.ttf", 32.f);
        
        // NOTE(diego): Added to handle multithreaded work.
        platform_add_entry = memory->platform_add_entry;
        platform_complete_all_work = memory->platform_complete_all_work;
        
        // @Cleanup
        //
        // Pass these inside App_State?
        // Make App_State visible and accessible to games?
        //
        menu_title_font = load_font("./data/fonts/Inconsolata-Bold.ttf", 48.f);
        menu_item_font = load_font("./data/fonts/Inconsolata-Bold.ttf", 36.f);
        
        load_menu_arts(state);
        
        hotloader_register_callback(my_hotloader_callback);
        
        state->initialized = true;
    }
    
    if (state->current_mode == Mode_SelectingGame) {
        update_mode_selecting(state, input);
    }
    
    update_debug(input);
    
    // Draw
    immediate_begin();
    App_Mode m = state->current_mode;
    if (m == Mode_SelectingGame) {
        render_mode_selecting(state);
    } else if (m == Mode_PlayingGame) {
        if (memory->asked_to_quit) {
            game_quit(state, memory);
        } else {
            game_table[state->current_game](memory, input);
        }
        immediate_flush();
        
        draw_framebuffer(state->window_dimensions);
        
        if (debug_draw.draw_mixer) {
            draw_debug_draw_mixer(state->window_dimensions);
        }
    }
}

internal Simulate_Game
game_simulate(Game_Memory *memory, Game_Input *input, Game_Mode &game_mode) {
    Simulate_Game result = {};
    
    if (game_mode == GameMode_Playing) {
        if (pressed(Button_Escape)) {
            game_mode = GameMode_Menu;
        } else {
            result.operation = SimulateGameOp_Update;
        }
    } else if (game_mode == GameMode_Menu ||
               game_mode == GameMode_GameOver) {
        if (pressed(Button_Down)) {
            advance_menu_choice(&memory->menu_selected_item, 1);
        }
        if (pressed(Button_Up)) {
            advance_menu_choice(&memory->menu_selected_item, -1);
        }
        if (pressed(Button_Escape)) {
            if (game_mode == GameMode_GameOver) {
                memory->asked_to_quit = true;
            } else {
                game_mode = GameMode_Playing;
            }
        }
        if (pressed(Button_Enter)) {
            switch (memory->menu_selected_item) {
                case 0: {
                    result.operation = SimulateGameOp_Restart;
                } break;
                
                case 1: {
                    if (memory->quit_was_selected) {
                        memory->asked_to_quit = true;
                    } else {
                        memory->quit_was_selected = true;
                    }
                } break;
                
                default: {
                    assert(!"Should not happen!");
                } break;
            }
        }
        if (memory->menu_selected_item != 1) {
            memory->quit_was_selected = false;
        } else if (memory->quit_was_selected) {
            if (pressed(Button_Escape)) {
                memory->quit_was_selected = false;
            }
        }
    }
    
    return result;
}

internal void
my_hotloader_callback(Asset_Change *change, b32 handled) {
    if (handled) return;
    
    // NOTE(diego): This is for assets that don't have a catalog.
}