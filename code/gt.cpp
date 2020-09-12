#include "gt.h"
#include "gt_opengl.cpp"
#include "gt_shader.cpp"
#include "gt_font.cpp"
#include "gt_draw.cpp"

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


#include "games/dodger.cpp"
#include "games/memory_puzzle.cpp"

//
// Game titles
//
global_variable char* game_titles[] = {
    "",
    "Dodger",
    "Memory Puzzle",
};

//
// Game menu art table
//
void stub_menu_art(v2 min, v2 max) { assert(false); }
void (*menu_table[])(v2 min, v2 max) = {
    stub_menu_art,
    dodger_menu_art,
    memory_puzzle_menu_art,
};

// 
// Game update and render table
//
void stub_game_update_and_render(game_memory *memory, game_input *input) { assert(false); }

void (*game_table[])(game_memory *memory, game_input *input) = {
    stub_game_update_and_render,
    dodger_game_update_and_render,
    memory_puzzle_game_update_and_render,
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
}

// TODO(diego): Add sound
internal void
game_output_sound(game_sound_output_buffer *sound_buffer) {
    
}

internal void
update_mode_selecting(app_state *state, game_input *input) {
    
    if (pressed(Button_Left)) {
        advance_game(state, -1);
    }
    if (pressed(Button_Right)) {
        advance_game(state, 1);
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
    int selection_width = state->window_dimensions.width / (int) (Game_Count);
    real32 border_width = selection_width * 0.01f;
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
                immediate_quad(min, max, make_v4(1.f, .0f, 1.f, .2f), 1.f);
                
                // Draw select borders
                immediate_quad(left_min, left_max, make_v4(1.f, .0f, 1.f, 1.f), 1.f);
                immediate_quad(right_min, right_max, make_v4(1.f, .0f, 1.f, 1.f), 1.f);
                immediate_quad(top_min, top_max, make_v4(1.f, .0f, 1.f, 1.f), 1.f);
                immediate_quad(bottom_min, bottom_max, make_v4(1.f, .0f, 1.f, 1.f), 1.f);
                
                immediate_flush();
            }
        }
        char* title = game_titles[index];
        real32 size = get_text_width(&state->game_title_font, title);
        draw_text((min.x + max.x - size) / 2.f, max.height * 0.2f, (u8 *) title, &state->game_title_font, make_color(0xffffffff));
    }
}

internal void
game_update_and_render(app_state *state, game_memory *memory, game_input *input) {
    
    if (!state->initialized) {
        state->current_mode = Mode_SelectingGame;
        state->current_game = Game_None;
        state->game_title_font = load_font("./data/fonts/Inconsolata-Bold.ttf", 32.f);
        state->initialized = true;
    }
    
    if (state->current_mode == Mode_SelectingGame) {
        update_mode_selecting(state, input);
    }
    
    // Draw
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    
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

