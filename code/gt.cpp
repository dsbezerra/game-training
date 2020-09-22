#include "gt.h"
#include "gt_opengl.cpp"
#include "gt_shader.cpp"
#include "gt_font.cpp"
#include "gt_draw.cpp"

global_variable game_time_info time_info = {};

global_variable loaded_font menu_title_font;
global_variable loaded_font menu_item_font;

global_variable real32 global_press_t = .0f;

internal void *
game_alloc(game_memory *memory, u64 size) {
    memory->permanent_storage_size = size;
    memory->permanent_storage = platform_alloc(size);
    memory->initialized = true;
    return memory->permanent_storage;
}

internal void
game_free(game_memory *memory) {
    if (!memory) {
        return;
    }
    
    if (memory->permanent_storage) {
        platform_free(memory->permanent_storage);
    }
    memory->permanent_storage = 0;
    memory->permanent_storage_size = 0;
    memory->initialized = false;
    memory->asked_to_quit = false;
}

internal void
game_quit(app_state *state, game_memory *memory) {
    state->current_selecting_game = state->current_game;
    state->current_mode = Mode_SelectingGame;
    game_free(memory);
}

internal void
advance_menu_choice(s8 *current_choice, s8 delta) {
    *current_choice += delta;
    
    if (*current_choice < 0) *current_choice += (s8) GameMenuItem_Count;
    if (*current_choice >= (s8) GameMenuItem_Count) *current_choice -= (s8) GameMenuItem_Count;
}

internal void
draw_menu(char *game_title, v2i dim, game_mode mode, s8 menu_selected_item, b32 quit_was_selected) {
    
    v4 white          = make_color(0xffffffff);
    v4 default_color  = make_color(0xffaaaaaa);
    v4 selected_color = make_color(0xffffff00);
    
    char *menu_title = mode == GameMode_GameOver ? "Game Over" : game_title;
    real32 menu_title_width = get_text_width(&menu_title_font, menu_title);
    
    real32 y = dim.height * 0.2f;
    
    //
    // Title
    //
    //  Retry
    //  Quit
    //
    
    
    draw_text((dim.width - menu_title_width) / 2.f, y, (u8 *) menu_title, &menu_title_font, white);
    
    char* menu_items[] = {"Retry", quit_was_selected ? "Quit? Are you sure?" : "Quit"};
    
    y += dim.height * 0.25f;
    for (int menu_item = 0; menu_item < array_count(menu_items); ++menu_item) {
        
        char *text = menu_items[menu_item];
        real32 width = get_text_width(&menu_item_font, text);
        
        if (menu_selected_item == menu_item) {
            v4 non_white = make_color(0xffffde00);
            
            real32 now = cosf(time_info.current_time);
            real32 t = cosf(now * 3);
            t *= t;
            
            t = .4f + .54f * t;
            v4 front_color = lerp_color(non_white, t, white);
            
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

#include "games/dodger.cpp"
#include "games/memory_puzzle.cpp"
#include "games/slide_puzzle.cpp"
#include "games/simon.cpp"
#include "games/nibbles.cpp"

//
// Game titles
//
global_variable char* game_titles[] = {
    "",
    "Dodger",
    "Memory Puzzle",
    "Slide Puzzle",
    "Simon",
    "Nibbles",
};

//
// Game menu art table
//
void stub_menu_art(v2 min, v2 max) { assert(false); }
void (*menu_table[])(v2 min, v2 max) = {
    stub_menu_art,
    dodger_menu_art,
    memory_puzzle_menu_art, // TODO(diego): Update menu art routine for for Memory Puzzle
    slide_puzzle_menu_art,
    simon_menu_art,
    nibbles_menu_art,
};

// 
// Game update and render table
//
void stub_game_update_and_render(game_memory *memory, game_input *input) { assert(false); }

void (*game_table[])(game_memory *memory, game_input *input) = {
    stub_game_update_and_render,
    dodger_game_update_and_render,
    memory_puzzle_game_update_and_render,
    slide_puzzle_game_update_and_render,
    simon_game_update_and_render,
};

internal void
advance_game(app_state *state, int value) {
    int new_game = ((int) state->current_selecting_game) + value;
    if (new_game < Game_None) {
        new_game = (int) (Game_Count - 1);
    } else if (new_game >= Game_Count) {
        new_game = (int) Game_None;
    }
    state->current_selecting_game = (game) new_game;
    global_press_t = .0f;
}

// TODO(diego): Add sound
internal void
game_output_sound(game_sound_output_buffer *sound_buffer) {
    
}

internal void
update_mode_selecting(app_state *state, game_input *input) {
    
    real32 press_t_target = 0.2f;
    
    if (pressed(Button_Left)) {
        advance_game(state, -1);
    } else if (is_down(Button_Left)) {
        if (global_press_t >= press_t_target) {
            advance_game(state, -1);
        }
        global_press_t += time_info.dt;
    }
    if (pressed(Button_Right)) {
        advance_game(state, 1);
    } else if (is_down(Button_Right)) {
        if (global_press_t >= press_t_target) {
            advance_game(state, 1);
        }
        global_press_t += time_info.dt;
    }
    if (pressed(Button_Enter)) {
        if (state->current_selecting_game > Game_None && state->current_selecting_game < Game_Count) {
            if (state->current_selecting_game != state->current_game) {
                state->current_game = state->current_selecting_game;
                state->current_selecting_game = Game_None;
                game_free(state->memory);
            }
            state->current_mode = Mode_PlayingGame;
        }
    }
    
    if (pressed(Button_Escape) && state->current_game != Game_None) {
        state->current_mode = Mode_PlayingGame;
    }
}

internal void
render_mode_selecting(app_state *state) {
    int selection_width = state->window_dimensions.width / 4;
    real32 border_width = selection_width * 0.01f;
    
    real32 now = time_info.current_time;
    real32 t = cosf(now);
    t *= t;
    t = .4f + t;
    
    v4 border_color = make_v4(1.f, .0f, 1.f, .4f);
    v4 border_color_almost_transparent = make_v4(1.f, .0f, 1.f, .2f);
    
    real32 offset = (real32) (selection_width * (int) state->current_selecting_game);
    mat4 translation = translate(make_v2(-offset + state->window_dimensions.width * .5f, .0f));
    
#if 0
    real32 tCos = cosf(time_info.current_time);
    tCos *= tCos;
    tCos = .95f + .05f * tCos;
    mat4 center = translate(make_v2(state->window_dimensions.width * 0.5f - offset, .0f));
    view_matrix = view_matrix * center * scale(make_v2(tCos, tCos));
#endif
    
#if 1
    view_matrix = view_matrix * translation;
#endif
    
    refresh_shader_transform();
    
    for (int index = 0; index < array_count(game_titles); ++index) {
        real32 x = (real32) index * selection_width;
        v2 min  = make_v2(x, 0.f);
        v2 max = make_v2(x + selection_width, (real32) state->window_dimensions.height);
        
        if (index == 0) {
            // Do nothing
        } else {
            if (index < array_count(menu_table)) {
                menu_table[index](min, max);
            }
            if (index == (int) state->current_selecting_game) {
                
                v2 left_min = make_v2(min.x, min.y);
                v2 left_max = make_v2(min.x + border_width, max.y);
                
                v2 right_min = make_v2(max.x - border_width, min.y);
                v2 right_max = make_v2(max.x, max.y);
                
                v2 top_min = make_v2(min.x, min.y);
                v2 top_max = make_v2(max.x, min.y + border_width);
                
                v2 bottom_min = make_v2(min.x, max.y - border_width);
                v2 bottom_max = make_v2(max.x, max.y);
                
                immediate_begin();
                
                // Draw select quad
                
                v4 a = make_v4(1.f, .0f, 1.f, .4f);
                v4 b = make_v4(1.f, .0f, 1.f, .0f);
                immediate_quad(min, max, lerp_color(b, t, a), 1.f);
                
                // Draw select borders
                v4 border_t_color = lerp_color(border_color, t, border_color_almost_transparent);
                immediate_quad(left_min, left_max, border_t_color, 1.f);
                immediate_quad(right_min, right_max, border_t_color, 1.f);
                immediate_quad(top_min, top_max, border_t_color, 1.f);
                immediate_quad(bottom_min, bottom_max, border_t_color, 1.f);
                
                immediate_flush();
            }
        }
        char* title = game_titles[index];
        real32 size = get_text_width(&state->game_title_font, title);
        draw_text((min.x + max.x - size) * .5f, max.height * 0.2f, (u8 *) title, &state->game_title_font, make_color(0xffffffff));
    }
}

internal void
game_update_and_render(app_state *state, game_memory *memory, game_input *input) {
    
    if (!state->initialized) {
        state->current_mode = Mode_SelectingGame;
        state->current_game = Game_None;
        state->game_title_font = load_font("./data/fonts/Inconsolata-Bold.ttf", 32.f);
        
        // @Cleanup
        //
        // Pass theses inside app_state?
        // Make app_state visible and accessible to games?
        //
        menu_title_font = load_font("./data/fonts/Inconsolata-Bold.ttf", 48.f);
        menu_item_font = load_font("./data/fonts/Inconsolata-Bold.ttf", 36.f);
        
        
        state->initialized = true;
    }
    
    if (state->current_mode == Mode_SelectingGame) {
        update_mode_selecting(state, input);
    }
    
    // Draw
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    GLsizei width = (GLsizei) state->window_dimensions.x;
    GLsizei height = (GLsizei) state->window_dimensions.y;
    glViewport(0, 0, width, height);
    
    render_right_handed(width, height);
    
    immediate_begin();
    mode m = state->current_mode;
    if (m == Mode_SelectingGame) {
        render_mode_selecting(state);
    } else if (m == Mode_PlayingGame) {
        if (memory->asked_to_quit) {
            game_quit(state, memory);
        } else {
            game_table[state->current_game](memory, input);
        }
        
    }
    immediate_flush();
}

