internal void
init_game(Connect_Four_State *state) {
    state->game_mode = GameMode_Playing;
    state->memory->game_mode = GameMode_Playing;
    
    platform_show_cursor(true);
}

internal void
update_game(Connect_Four_State *state, Game_Input *input) {
    // TODO(diego): Implement.
}

internal void
clear_board(Connect_Four_Board *board) {
    for (u32 x = 0; x < CONNECT_FOUR_X_COUNT; ++x) {
        for (u32 y = 0; y < CONNECT_FOUR_Y_COUNT; ++y) {
            Connect_Four_Tile *tile = &board->tiles[x][y];
            tile->kind = ConnectFourTileKind_None;
            tile->x = x;
            tile->y = y;
        }
    }
}

internal void
draw_game_view(Connect_Four_State *state) {
    
    Vector2i dim = state->memory->window_dimensions;
    
    game_frame_begin(dim.width, dim.height);
    
    if (state->game_mode == GameMode_Playing) {
        immediate_begin();
        immediate_quad(0.f, 0.f, (real32) dim.width, (real32) dim.height, make_color(0xff2f3242));
        immediate_flush();
        
        draw_board(state);
    } else {
        draw_menu(CONNECT_FOUR_TITLE, state->memory);
    }
}

internal void
draw_board(Connect_Four_State *state) {
    
    Vector2i dim = state->memory->window_dimensions;
    
    u32 max_size = dim.width > dim.height ? dim.height : dim.width;
    real32 tile_size = max_size * 0.1f;
    
    real32 x_width  = CONNECT_FOUR_X_COUNT * tile_size;
    real32 y_height = CONNECT_FOUR_Y_COUNT * tile_size;
    
    real32 sx = center_horizontally((real32) dim.width, x_width);
    real32 sy = center_vertically((real32) dim.height, y_height);
    
    Vector4 board_color = make_color(0xFF7985FF);
    Vector4 tile_red = make_color(0xFFB01212);
    Vector4 tile_black = make_color(0xFF222323);
    Vector4 background = make_color(0xff2f3242);
    Vector4 tile_black2 = make_color(0xFF000000);
    
    immediate_begin();
    
    //
    // Board
    //
    {
        Vector2 min = make_vector2(sx, sy);
        Vector2 max = make_vector2(min.x + x_width, sy + y_height);
        immediate_quad(min, max, board_color);
    }
    
    //
    // Tiles
    //
    u32 line = 0;
    for (u32 x = 0; x < CONNECT_FOUR_X_COUNT; ++x) {
        for (u32 y = 0; y < CONNECT_FOUR_Y_COUNT; ++y) {
            Connect_Four_Tile tile = state->board.tiles[x][y];
            
            Vector4 color;
            switch (tile.kind) {
                case ConnectFourTileKind_Red: {
                    color = tile_red;
                } break;
                case ConnectFourTileKind_Black: {
                    color = tile_black;
                } break;
                default: color = background; break;
            }
            
            real32 start_x = sx + x * tile_size;
            real32 start_y = sy + y * tile_size;
            Vector2 center = make_vector2(start_x + tile_size * .5f, start_y + tile_size * .5f);
            
            real32 radius = tile_size * 0.44f;
            immediate_circle_filled(center, radius, color);
        }
        
        if (line < 5) {
            line++;
            real32 line_y = sy + line * tile_size;
            Vector2 min = make_vector2(sx, line_y - 1.f);
            Vector2 max = make_vector2(min.x + x_width, min.y + 1.f);
            immediate_quad(min, max, tile_black2);
        }
    }
    
    immediate_flush();
}

internal void
draw_hud(Connect_Four_State *state) {
    // TODO(diego): Implement.
}

internal void
connect_four_menu_art(App_State *state, Vector2 min, Vector2 max) {
    immediate_begin();
    immediate_quad(min, max, make_color(0xffff00ff));
    immediate_flush();
}

internal void
connect_four_game_restart(Connect_Four_State *state) {
    init_game(state);
}

internal void
connect_four_game_update_and_render(Game_Memory *memory, Game_Input *input) {
    Connect_Four_State *state = (Connect_Four_State *) memory->permanent_storage;
    if (!memory->initialized) {
        assert(memory->permanent_storage_size == 0);
        assert(!memory->permanent_storage);
        memory->initialized = true;
        
        // NOTE(diego): Not used.
        Memory_Index total_memory_size = kilobytes(16);
        state = (Connect_Four_State *) game_alloc(memory, total_memory_size);
        state->memory = memory;
        
        init_game(state);
    }
    
    Simulate_Game sim = game_simulate(memory, input, state->game_mode);
    switch (sim.operation) {
        case SimulateGameOp_Update: {
            update_game(state, input);
        } break;
        
        case SimulateGameOp_Restart: {
            connect_four_game_restart(state);
        } break;
        
        default: break;
    }
    
    //
    // Draw
    //
    draw_game_view(state);
}

internal void
connect_four_game_free(Game_Memory *memory) {
    if (!memory) return;
    
    Connect_Four_State *state = (Connect_Four_State *) memory->permanent_storage;
    if (!state) return;
    
    // TODO(diego): If necessary.
}
