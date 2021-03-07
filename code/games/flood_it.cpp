internal void
init_game(Flood_It_State *state) {
    state->game_mode = GameMode_Playing;
    generate_grid(state);
}

internal void
update_game(Flood_It_State *state, Game_Input *input) {
    // TODO(diego): Implement.
}

internal void
generate_grid(Flood_It_State *state) {
    Flood_It_Grid *grid = &state->grid;
    for (u32 x = 0; x < FLOOD_IT_GRID_SIZE; ++x) {
        for (u32 y = 0; y < FLOOD_IT_GRID_SIZE; ++y) {
            grid->tiles[x][y] = make_random_tile(x, y);
        }
    }
}

internal Flood_It_Tile
make_random_tile(u32 x, u32 y) {
    assert(x >= 0 && x < FLOOD_IT_GRID_SIZE);
    assert(y >= 0 && y < FLOOD_IT_GRID_SIZE);
    
    Flood_It_Tile result = {};
    
    Flood_It_Tile_Kind kind;
    
    real32 t = random_real32_in_range(0.f, 2.f);
    if (t >= 2.f) {
        kind = FloodItTileKind_Red;
    } else if (t >= 1.6f) {
        kind = FloodItTileKind_Green;
    } else if (t >= 1.3f) {
        kind = FloodItTileKind_Blue;
    } else if (t >= 0.9f) {
        kind = FloodItTileKind_Yellow;
    } else if (t >= 0.4f) {
        kind = FloodItTileKind_Orange;
    } else {
        kind = FloodItTileKind_Pink;
    }
    result.x = x;
    result.y = y;
    result.kind = kind;
    result.color = get_color_for_tile(kind);
    
    return result;
}

internal Vector4
get_color_for_tile(Flood_It_Tile_Kind kind) {
    Vector4 result = {};
    
    switch (kind) {
        case FloodItTileKind_Red:    result = make_color(0xffff0000); break;
        case FloodItTileKind_Green:  result = make_color(0xff00ff00); break;
        case FloodItTileKind_Blue:   result = make_color(0xff0000ff); break;
        case FloodItTileKind_Yellow: result = make_color(0xffffff00); break;
        case FloodItTileKind_Orange: result = make_color(0xffff8200); break;
        case FloodItTileKind_Pink:   result = make_color(0xffff00ff); break;
        default: invalid_code_path; break;
    }
    
    return result;
}


internal void
draw_grid(Flood_It_State *state) {
    
    Vector2i dim = state->dimensions;
    
    real32 height = dim.height * .7f;
    real32 tile_size = height / (real32) FLOOD_IT_GRID_SIZE;
    
    real32 width  = tile_size * OTHELLO_BOARD_COUNT;
    real32 remaining_wspace = dim.width - width;
    real32 remaining_hspace = dim.height - height;
    
    real32 start_x = remaining_wspace * .5f - width * .5f;
    real32 start_y = remaining_hspace * .5f;
    
    immediate_begin();
    
    for (u32 tile_x = 0; tile_x < FLOOD_IT_GRID_SIZE; ++tile_x) {
        for (u32 tile_y = 0; tile_y < FLOOD_IT_GRID_SIZE; ++tile_y) {
            Flood_It_Tile tile = state->grid.tiles[tile_x][tile_y];
            if (tile.kind == FloodItTileKind_None) continue;
            
            real32 sx = start_x + tile_x * tile_size;
            real32 sy = start_y + tile_y * tile_size;
            
            Vector2 min = make_vector2(sx, sy);
            Vector2 max = make_vector2(sx + tile_size, sy + tile_size);
            
            immediate_quad(min, max, tile.color);
        }
    }
    
    immediate_flush();
}

internal void
draw_game_view(Flood_It_State *state) {
    
    game_frame_begin(state->dimensions.width, state->dimensions.height);
    
    if (state->game_mode == GameMode_Playing) {
        Vector2i dim = state->dimensions;
        
        // TODO(diego): Replace background
        immediate_begin();
        immediate_quad(0.f, 0.f, (real32) dim.width, (real32) dim.height, make_color(0xff2f3242));
        immediate_flush();
        
        draw_grid(state);
        draw_hud(state);
    } else {
        draw_menu(FLOOD_IT_TITLE, state->dimensions, state->game_mode, state->menu_selected_item, state->quit_was_selected);
    }
}

internal void
draw_hud(Flood_It_State *state) {
    // TODO(diego): Implement.
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
        
        // TODO(diego): Implement.
        
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
