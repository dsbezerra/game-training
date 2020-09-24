internal void
draw_game_view(tetris_state *state) {
    if (state->game_mode == GameMode_Playing) {
    } else {
        draw_menu(TETRIS_TITLE, state->dimensions, state->game_mode, state->menu_selected_item, state->quit_was_selected);
    }
}

internal void
tetris_menu_art(v2 min, v2 max) {
    v4 background = make_color(0xff5a1c5f);
    immediate_begin();
    immediate_quad(min, max, background, 1.f);
    immediate_flush();
}

internal void
tetris_game_restart(tetris_state *state) {
    // TODO(diego): Add reinit code.
}

internal void
tetris_game_update_and_render(game_memory *memory, game_input *input) {
    
    tetris_state *state = (tetris_state *) memory->permanent_storage;
    if (!memory->initialized) {
        assert(memory->permanent_storage_size == 0);
        assert(!memory->permanent_storage);
        memory->initialized = true;
        
        state = (tetris_state *) game_alloc(memory, megabytes(12));
    }
    state->dimensions = memory->window_dimensions;
    
    //
    // Update
    //
    if (state->game_mode == GameMode_Playing) {
        if (pressed(Button_Escape)) {
            state->game_mode = GameMode_Menu;
        } else {
            // TODO(diego): Implement update
        }
    } else if (state->game_mode == GameMode_Menu || state->game_mode == GameMode_GameOver) {
        if (pressed(Button_Down)) {
            advance_menu_choice(&state->menu_selected_item, 1);
        }
        if (pressed(Button_Up)) {
            advance_menu_choice(&state->menu_selected_item, -1);
        }
        if (pressed(Button_Escape)) {
            if (state->game_mode == GameMode_GameOver) {
                memory->asked_to_quit = true;
            } else {
                state->game_mode = GameMode_Playing;
            }
        }
        
        if (pressed(Button_Enter)) {
            switch (state->menu_selected_item) {
                case 0: {
                    tetris_game_restart(state);
                } break;
                
                case 1: {
                    if (state->quit_was_selected) {
                        memory->asked_to_quit = true;
                    } else {
                        state->quit_was_selected = true;
                    }
                } break;
                
                default: {
                    assert(!"Should not happen!");
                } break;
            }
        }
        
        if (state->menu_selected_item != 1) {
            state->quit_was_selected = false;
        } else if (state->quit_was_selected) {
            if (pressed(Button_Escape)) {
                state->quit_was_selected = false;
            }
        }
    }
    
    //
    // Draw
    //
    
    draw_game_view(state);
}
