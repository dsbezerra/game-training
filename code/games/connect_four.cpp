internal void
init_game(Connect_Four_State *state) {
    state->game_mode = GameMode_Playing;
    state->memory->game_mode = GameMode_Playing;
    state->player = ConnectFourTileKind_Red;
    state->ai_player = ConnectFourTileKind_Black;
    state->current_player = ConnectFourTileKind_Red;
    
    platform_show_cursor(true);
    clear_board(&state->board);
}

internal void
update_game(Connect_Four_State *state, Game_Input *input) {
    if (state->current_player == state->player) {
        update_dragging_tile(state, input);
    } else if (state->current_player == state->ai_player) {
        best_move(&state->board, state->ai_player);
    }
}

internal void
update_dragging_tile(Connect_Four_State *state, Game_Input *input) {
    platform_get_cursor_position(&state->mouse_position);
    
    Connect_Four_Tile *tile = &state->dragging_tile;
    if (tile->kind != ConnectFourTileKind_None) {
        b32 released = released(Button_Mouse1);
        if (released) {
            set_tile(tile, ConnectFourTileKind_None);
        }
        
        Vector2i dim = state->memory->window_dimensions;
        real32 tile_size = get_tile_size(dim);
        
        real32 x_width  = CONNECT_FOUR_X_COUNT * tile_size;
        real32 y_height = CONNECT_FOUR_Y_COUNT * tile_size;
        
        real32 sx = center_horizontally((real32) dim.width, x_width);
        real32 sy = center_vertically((real32) dim.height, y_height);
        
        real32 max_x = sx + x_width;
        
        Vector2i mp = state->mouse_position;
        if (mp.x < sx || mp.x > max_x || mp.y > sy - tile_size*.5f) {
            tile->board_x = -1;
            tile->board_y = -1;
            return;
        }
        
        s32 x = (s32) ((mp.x - sx) / tile_size);
        assert(x >= 0 && x < CONNECT_FOUR_X_COUNT);
        
        tile->board_x = x;
        tile->board_y = 0; // NOTE(diego): Always 0
        
        if (released) {
            if (is_valid_move(&state->board, x)) {
                make_move(&state->board, state->player, x);
                if (check_win(&state->board, state->player)) {
                    connect_four_game_restart(state);
                } else {
                    switch_turns(state);
                }
            }
        }
        
    } else {
        if (pressed(Button_Mouse1)) {
            if (is_inside_player(state)) {
                set_tile(&state->dragging_tile, state->player);
            }
        }
    }
}

internal void
clear_board(Connect_Four_Board *board) {
    Vector2i dim = board->state->memory->window_dimensions;
    real32 tile_size = get_tile_size(dim);
    
    real32 x_width  = CONNECT_FOUR_X_COUNT * tile_size;
    real32 y_height = CONNECT_FOUR_Y_COUNT * tile_size;
    
    real32 sx = center_horizontally((real32) dim.width, x_width);
    real32 sy = center_vertically((real32) dim.height, y_height);
    
    Vector4 background = make_color(0xff2f3242);
    
    for (u32 x = 0; x < CONNECT_FOUR_X_COUNT; ++x) {
        for (u32 y = 0; y < CONNECT_FOUR_Y_COUNT; ++y) {
            real32 start_x = sx + x * tile_size;
            real32 start_y = sy + y * tile_size;
            
            Connect_Four_Tile *tile = &board->tiles[x][y];
            tile->kind = ConnectFourTileKind_None;
            tile->board_x = x;
            tile->board_y = y;
            tile->min     = make_vector2(start_x, start_y);
            tile->max     = make_vector2(tile->min.x + tile_size, tile->min.y + tile_size);
            tile->size    = make_vector2(tile_size, tile_size);
            tile->center  = tile->min + tile->size * .5f;
            tile->color   = background;
            tile->radius  = tile_size * .44f;
        }
    }
    
    board->filled = 0;
}

internal void
set_tile(Connect_Four_Board *board, Connect_Four_Tile_Kind kind, u32 tile_x, u32 tile_y) {
    assert(tile_x >= 0 && tile_x < CONNECT_FOUR_X_COUNT);
    assert(tile_y >= 0 && tile_y < CONNECT_FOUR_Y_COUNT);
    
    Connect_Four_Tile *tile = &board->tiles[tile_x][tile_y];
    set_tile(tile, kind);
}

internal void
set_tile(Connect_Four_Tile *tile, Connect_Four_Tile_Kind kind) {
    tile->kind = kind;
    tile->color = get_tile_color(kind);
}

internal void
switch_turns(Connect_Four_State *state) {
    if (state->play_state == ConnectFourPlayState_RedTurn) {
        state->play_state = ConnectFourPlayState_BlackTurn;
        state->current_player = ConnectFourTileKind_Black;
    } else if (state->play_state == ConnectFourPlayState_BlackTurn) {
        state->play_state = ConnectFourPlayState_RedTurn;
        state->current_player = ConnectFourTileKind_Red;
    }
}

internal void
make_move(Connect_Four_Board *board, Connect_Four_Tile_Kind kind, s32 x) {
    Connect_Four_Move empty = get_lowest_empty_move(board, x);
    set_tile(board, kind, empty.x, empty.y);
    board->filled++;
}

internal void
copy_board(Connect_Four_Board *dest, Connect_Four_Board *src) {
    dest->state = src->state;
    for (s32 x = 0; x < CONNECT_FOUR_X_COUNT; ++x) {
        for (s32 y = 0; y < CONNECT_FOUR_Y_COUNT; ++y) {
            dest->tiles[x][y] = src->tiles[x][y];
        }
    }
    dest->filled = src->filled;
}


internal Connect_Four_Move
get_lowest_empty_move(Connect_Four_Board *board, s32 x) {
    Connect_Four_Move result = {};
    
    for (s32 y = CONNECT_FOUR_Y_COUNT - 1; y >= 0; --y) {
        if (is_tile_empty(board, x, y)) {
            result.x = x;
            result.y = y;
            break;
        }
    }
    
    return result;
}

internal b32
is_tile_empty(Connect_Four_Board *board, u32 tile_x, u32 tile_y) {
    assert(board);
    
    Connect_Four_Tile_Kind result;
    
    result = board->tiles[tile_x][tile_y].kind;
    
    return result == ConnectFourTileKind_None;
}

internal b32
is_inside_player(Connect_Four_State *state) {
    Vector2i dim = state->memory->window_dimensions;
    
    real32 center_x = dim.width  * .10f;
    real32 center_y = dim.height * .9f;
    
    real32 tile_size = get_tile_size(dim);
    real32 radius = tile_size * .44f;
    
    real32 dx = state->mouse_position.x - center_x;
    real32 dy = state->mouse_position.y - center_y; 
    
    return dx*dx + dy*dy <= radius*radius;
}

internal b32
is_board_full(Connect_Four_Board *board) {
#if 0
    for (u32 x = 0; x < CONNECT_FOUR_X_COUNT; ++x) {
        for (u32 y = 0; y < CONNECT_FOUR_Y_COUNT; ++y) {
            if (is_tile_empty(board, x, y)) {
                return false;
            }
        }
    }
    return true;
#else
    return board->filled >= CONNECT_FOUR_X_COUNT*CONNECT_FOUR_Y_COUNT;
#endif
}

internal b32
is_valid_move(Connect_Four_Board *board, s32 x) {
    if (x < 0 || x >= CONNECT_FOUR_X_COUNT) return false;
    return board->tiles[x][0].kind == ConnectFourTileKind_None;
}

internal real32
get_tile_size(Vector2i dimensions) {
    u32 max_size = dimensions.width > dimensions.height ? dimensions.height : dimensions.width;
    real32 tile_size = max_size * 0.1f;
    return tile_size;
}

internal Vector4
get_tile_color(Connect_Four_Tile_Kind kind) {
    Vector4 result = {};
    
    switch (kind) {
        case ConnectFourTileKind_Red: {
            result = make_color(0xFFB01212);
        } break;
        
        case ConnectFourTileKind_Black: {
            result = make_color(0xFF222323);
        } break;
        
        case ConnectFourTileKind_None: {
            result = make_color(0xff2f3242);
        } break;
        
        default: invalid_code_path; break;
    }
    
    return result;
}

internal b32
has_n_connected(Connect_Four_Board *board, Connect_Four_Tile_Kind kind, u32 start_x, u32 start_y, u32 advance_x, u32 advance_y, u32 n) {
    
    if (start_x < 0 && start_x >= CONNECT_FOUR_X_COUNT) return false;
    if (start_y < 0 && start_y >= CONNECT_FOUR_Y_COUNT) return false;
    if (advance_x == 0 && advance_y == 0) return false;
    if (n == 0) return false;
    
    u32 x = start_x;
    u32 y = start_y;
    
    Connect_Four_Tile *start_tile = &board->tiles[x][y];
    if (start_tile->kind == ConnectFourTileKind_None) return false;
    
    u32 connected = 1;
    while (1) {
        x += advance_x;
        y += advance_y;
        
        if (x >= CONNECT_FOUR_X_COUNT) break;
        if (y >= CONNECT_FOUR_Y_COUNT) break;
        
        Connect_Four_Tile *tile = &board->tiles[x][y];
        if (tile->kind == start_tile->kind) {
            connected++;
        } else {
            break;
        }
        
        if (connected == n) break;
    }
    
    return connected == n;
}

internal b32
has_four_connected(Connect_Four_Board *board, Connect_Four_Tile_Kind kind, u32 start_x, u32 start_y, u32 advance_x, u32 advance_y) {
    return has_n_connected(board, kind, start_x, start_y, advance_x, advance_y, 4);
}

internal Connect_Four_Winner
check_win(Connect_Four_Board *board) {
    if (check_win(board, ConnectFourTileKind_Black)) {
        return ConnectFourWinner_Black;
    }
    if (check_win(board, ConnectFourTileKind_Red)) {
        return ConnectFourWinner_Red;
    }
    return ConnectFourWinner_None;
}

internal b32
check_win(Connect_Four_Board *board, Connect_Four_Tile_Kind kind) {
    
    // Check horizontal
    for (u32 x = 0; x < CONNECT_FOUR_X_COUNT - 3; ++x) {
        for (u32 y = 0; y < CONNECT_FOUR_Y_COUNT; ++y) {
            if (board->tiles[  x  ][y].kind == kind &&
                board->tiles[x + 1][y].kind == kind &&
                board->tiles[x + 2][y].kind == kind &&
                board->tiles[x + 3][y].kind == kind) {
                return true;
            }
        }
    }
    
    // Check vertical
    for (u32 x = 0; x < CONNECT_FOUR_X_COUNT; ++x) {
        for (u32 y = 0; y < CONNECT_FOUR_Y_COUNT - 3; ++y) {
            if (board->tiles[x][  y  ].kind == kind &&
                board->tiles[x][y + 1].kind == kind &&
                board->tiles[x][y + 2].kind == kind &&
                board->tiles[x][y + 3].kind == kind) {
                return true;
            }
        }
    }
    
    // Check / diagonal
    for (u32 x = 0; x < CONNECT_FOUR_X_COUNT - 3; ++x) {
        for (u32 y = 3; y < CONNECT_FOUR_Y_COUNT; ++y) {
            if (board->tiles[  x  ][  y  ].kind == kind &&
                board->tiles[x + 1][y - 1].kind == kind &&
                board->tiles[x + 2][y - 2].kind == kind &&
                board->tiles[x + 3][y - 3].kind == kind) {
                return true;
            }
        }
    }
    
    // Check \ diagonal 
    for (u32 x = 0; x < CONNECT_FOUR_X_COUNT - 3; ++x) {
        for (u32 y = 0; y < CONNECT_FOUR_Y_COUNT - 3; ++y) {
            if (board->tiles[  x  ][  y  ].kind == kind &&
                board->tiles[x + 1][y + 1].kind == kind &&
                board->tiles[x + 2][y + 2].kind == kind &&
                board->tiles[x + 3][y + 3].kind == kind) {
                return true;
            }
        }
    }
    
    return false;
}

#define CONNECT_FOUR_SEARCH_DEPTH 2

internal void
best_move(Connect_Four_Board *board, Connect_Four_Tile_Kind player) {
    Memory_Arena *arena = &board->state->moves_arena;
    
    reset_arena(arena);
    
    s32 potential_moves[CONNECT_FOUR_X_COUNT] = {};
    minimax(board, CONNECT_FOUR_SEARCH_DEPTH, potential_moves);
    
    s32 best_move = -1;
    for (u32 move = 0; move < CONNECT_FOUR_X_COUNT; ++move) {
        if (potential_moves[move] > best_move && is_valid_move(board, move)) {
            best_move = potential_moves[move];
        }
    }
    
    u32 num_moves = 0;
    for (u32 move = 0; move < CONNECT_FOUR_X_COUNT; ++move) {
        if (potential_moves[move] == best_move && is_valid_move(board, move)) {
            num_moves++;
        }
    }
    
    s32 *best_moves = push_array(arena, num_moves, s32);
    s32 best_move_index = 0;
    for (u32 move = 0; move < CONNECT_FOUR_X_COUNT; ++move) {
        if (potential_moves[move] == best_move && is_valid_move(board, move)) {
            best_moves[best_move_index++] = move;
        }
    }
    
    best_move = best_moves[random_int_in_range(0, num_moves)];
    if (is_valid_move(board, best_move)) {
        make_move(board, player, best_move);
        if (check_win(board, player)) {
            connect_four_game_restart(board->state);
        } else {
            switch_turns(board->state);
        }
    }
}

// NOTE(diego): We could use alpha-beta pruning to make it faster.
internal void
minimax(Connect_Four_Board *board, u32 depth, s32 *potential_moves) {
    if (depth == 0 || is_board_full(board)) return;
    
    Memory_Arena *arena = &board->state->moves_arena;
    
    Connect_Four_Tile_Kind tile_kind = ConnectFourTileKind_Black;
    Connect_Four_Tile_Kind enemy_tile_kind = ConnectFourTileKind_Red;
    for (u32 first_move = 0; first_move < CONNECT_FOUR_X_COUNT; ++first_move) {
        if (!is_valid_move(board, first_move)) {
            continue;
        }
        
        Connect_Four_Board *board_copy = push_struct(arena, Connect_Four_Board);
        copy_board(board_copy, board);
        
        make_move(board_copy, tile_kind, first_move);
        if (check_win(board_copy, tile_kind)) {
            potential_moves[first_move] = 1;
            break;
        }
        
        if (is_board_full(board_copy)) {
            potential_moves[first_move] = 0;
        } else {
            for (u32 counter_move = 0; counter_move < CONNECT_FOUR_X_COUNT; ++counter_move) {
                if (!is_valid_move(board_copy, counter_move)) {
                    continue;
                }
                
                Connect_Four_Board *board_second_copy = push_struct(arena, Connect_Four_Board); copy_board(board_second_copy, board_copy);
                
                make_move(board_second_copy, enemy_tile_kind, counter_move);
                if (check_win(board_second_copy, enemy_tile_kind)) {
                    potential_moves[first_move] = -1;
                    break;
                }
                
                s32 *moves = push_array(arena, CONNECT_FOUR_X_COUNT, s32);
                minimax(board_second_copy, depth - 1, moves);
                
                s32 sum = 0;
                for (u32 s = 0; s < CONNECT_FOUR_X_COUNT; ++s)
                    sum += moves[s];
                
                potential_moves[first_move] += (sum / CONNECT_FOUR_X_COUNT) / CONNECT_FOUR_X_COUNT;
            }
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
        draw_hud(state);
    } else {
        draw_menu(CONNECT_FOUR_TITLE, state->memory);
    }
}

internal void
draw_board(Connect_Four_State *state) {
    
    Vector2i dim = state->memory->window_dimensions;
    
    u32 max_size = dim.width > dim.height ? dim.height : dim.width;
    real32 tile_size = get_tile_size(dim);
    real32 radius = tile_size * .44f;
    
    real32 x_width  = CONNECT_FOUR_X_COUNT * tile_size;
    real32 y_height = CONNECT_FOUR_Y_COUNT * tile_size;
    
    real32 sx = center_horizontally((real32) dim.width, x_width);
    real32 sy = center_vertically((real32) dim.height, y_height);
    
    Vector4 board_color = make_color(0xFF7985FF);
    Vector4 black = make_color(0xFF000000);
    
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
            immediate_circle_filled(tile.center, tile.radius, tile.color);
        }
        
        if (line < 5) {
            line++;
            real32 line_y = sy + line * tile_size;
            Vector2 min = make_vector2(sx, line_y - 1.f);
            Vector2 max = make_vector2(min.x + x_width, min.y + 1.f);
            immediate_quad(min, max, black);
        }
    }
    
    //
    // Hovering
    //
    Connect_Four_Tile ht = state->dragging_tile;
    if (ht.kind != ConnectFourTileKind_None) {
        
        Vector2 dragging = make_vector2((real32) state->mouse_position.x, (real32) state->mouse_position.y);
        
        // Draw row and place that it will land
        if (ht.board_x >= 0 && ht.board_x < CONNECT_FOUR_X_COUNT) {
            
            Connect_Four_Move move = get_lowest_empty_move(&state->board, ht.board_x);
            
            Vector2 min = make_vector2(sx + ht.board_x * tile_size, dragging.y);
            Vector2 max = make_vector2(min.x + tile_size, sy + move.y * tile_size + tile_size);
            
            Vector4 transparent = make_color(0x1afff931);
            Vector4 non_white = make_color(0xaafff931);
            
            real32 sin = sinf(core.time_info.current_time);
            real32 t = sin*sin;
            t = .4f + .54f * t;
            
            Vector4 color = lerp_color(non_white, t, transparent);
            immediate_quad(min, max, color);
            immediate_circle_filled(dragging, radius*1.15f, color);
            
            // Draw move
            {
                Vector2 center = make_vector2(sx + move.x * tile_size + tile_size *.5f, sy + move.y * tile_size + tile_size *.5f);
                
                real32 expand_size = radius * .2f;
                
                Vector4 place_color = lerp_color(ht.color, t, set_color_alpha(ht.color, .5f));
                immediate_circle_filled(center, radius + expand_size * (t * t), place_color);
            }
        }
        
        immediate_circle_filled(dragging, radius, ht.color);
    }
    
    immediate_flush();
}

internal void
draw_hud(Connect_Four_State *state) {
    
    Vector2i dim = state->memory->window_dimensions;
    
    immediate_begin();
    
    // Draw player
    {
        Vector4 color = get_tile_color(state->player);
        Vector2 center = make_vector2(dim.width * 0.10f, dim.height * 0.9f);
        
        real32 tile_size = get_tile_size(dim);
        real32 radius = tile_size * .44f;
        
        immediate_circle_filled(center, radius, color);
    }
    
    immediate_flush();
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
        
        Memory_Index moves_memory_size = megabytes(8);
        Memory_Index total_memory_size = moves_memory_size;
        Memory_Index total_available_size = total_memory_size - sizeof(Connect_Four_State);
        
        state = (Connect_Four_State *) game_alloc(memory, total_memory_size);
        state->memory = memory;
        state->board.state = state;
        
        init_arena(&state->moves_arena, total_available_size, (u8 *) memory->permanent_storage + sizeof(Connect_Four_State));
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
