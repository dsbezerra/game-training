internal void
init_game(Flood_It_State *state) {
    state->game_mode = GameMode_Playing;
}

internal void
update_game(Flood_It_State *state, Game_Input *input) {
    
}

internal void
draw_game_view(Flood_It_State *state) {
    
    game_frame_begin(state->dimensions.width, state->dimensions.height);
    
    if (state->game_mode == GameMode_Playing) {
        // TODO(diego): Replace background
        immediate_begin();
        immediate_quad(0.f, 0.f, (real32) state->dimensions.width, (real32) state->dimensions.height, make_color(0xff2f3242));
        immediate_flush();
        
        draw_hud(state);
    } else {
        draw_menu(FLOOD_IT_TITLE, state->dimensions, state->game_mode, state->menu_selected_item, state->quit_was_selected);
    }
}

internal void
draw_hud(Flood_It_State *state) {
}

internal void
flood_it_menu_art(App_State *state, Vector2 min, Vector2 max) {
    immediate_begin();
    immediate_quad(min, max, make_color(0xffff00ff));
    immediate_flush();
}

internal void
flood_it_game_restart(Flood_It_State *state) {
    init_game(state);
}

internal void
flood_it_game_update_and_render(Game_Memory *memory, Game_Input *input) {
    Flood_It_State *state = (Flood_It_State *) memory->permanent_storage;
    if (!memory->initialized) {
        assert(memory->permanent_storage_size == 0);
        assert(!memory->permanent_storage);
        memory->initialized = true;
        
        // NOTE(diego): Not used.
        Memory_Index total_memory_size = kilobytes(16);
        state = (Flood_It_State *) game_alloc(memory, total_memory_size);
        
        init_game(state);
    }
    
    state->dimensions = memory->window_dimensions;
    
    //
    // Update
    //
    if (state->game_mode == GameMode_Playing) {
        if (pressed(Button_Escape)) {
            state->game_mode = GameMode_Menu;
        } else {
            update_game(state, input);
        }
    } else if (state->game_mode == GameMode_Menu ||
               state->game_mode == GameMode_GameOver) {
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
                    flood_it_game_restart(state);
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

internal void
flood_it_game_free(Game_Memory *memory) {
    if (!memory) return;
    
    Flood_It_State *state = (Flood_It_State *) memory->permanent_storage;
    if (!state) return;
    
    // TODO(diego): If necessary.
}
