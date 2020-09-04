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
}


#include "games/dodger.cpp"
#include "games/memory_puzzle.cpp"

void stub_game_update_and_render(game_memory *memory, game_input *input) {
    assert(false);
}

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
    
    real32 selection_width = (real32) (state->window_dimensions.width / (int) (Game_Count));
    
    real32 x = (real32) ((int) state->current_selecting_game) * selection_width;
    
    v2 min = make_v2(x, 0.f);
    v2 max = make_v2(x + selection_width, (real32) state->window_dimensions.height);
    immediate_quad(min, max, make_v4(1.f, .0f, 1.f, 1.f), 1.f);
}

internal void
game_update_and_render(app_state *state, game_memory *memory, game_input *input) {
    
    if (!state->initialized) {
        state->current_mode = Mode_SelectingGame;
        state->current_game = Game_None;
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
        
        if (pressed(Button_Escape)) {
            state->current_selecting_game = state->current_game;
            state->current_mode = Mode_SelectingGame;
        }
        
        game_table[state->current_game](memory, input);
    }
    immediate_flush();
}

