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
    update_hovering_tile(state);
    
    if (state->current_player == state->player) {
        if (pressed(Button_Mouse1)) {
            make_move(state);
        }
    } else if (state->current_player == state->ai_player) {
        best_move(&state->board, state->ai_player);
    }
}

internal void
update_hovering_tile(Connect_Four_State *state) {
    Vector2i mouse_position;
    platform_get_cursor_position(&mouse_position);
    
    state->hovering_tile.kind = ConnectFourTileKind_None;
    state->hovering_tile.board_x = -1;
    state->hovering_tile.board_y = -1;
    
    Vector2i dim = state->memory->window_dimensions;
    real32 tile_size = get_tile_size(dim);
    
    real32 x_width  = CONNECT_FOUR_X_COUNT * tile_size;
    real32 y_height = CONNECT_FOUR_Y_COUNT * tile_size;
    
    real32 sx = center_horizontally((real32) dim.width, x_width);
    real32 sy = center_vertically((real32) dim.height, y_height);
    
    real32 max_x = sx + x_width;
    real32 max_y = sy + y_height;
    
    if (mouse_position.x < sx || mouse_position.x > max_x) return;
    if (mouse_position.y < sy || mouse_position.y > max_y) return;
    
    s32 x = (s32) ((mouse_position.x - sx) / tile_size);
    s32 y = (s32) ((mouse_position.y - sy) / tile_size);
    
    assert(x >= 0 && x < CONNECT_FOUR_X_COUNT);
    assert(y >= 0 && y < CONNECT_FOUR_Y_COUNT);
    
    state->hovering_tile = state->board.tiles[x][y];
    set_tile(&state->hovering_tile, state->player);
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
make_move(Connect_Four_State *state) {
    Connect_Four_Tile tile = state->hovering_tile;
    if (tile.kind != ConnectFourTileKind_None) {
        if (state->board.tiles[tile.board_x][tile.board_y].kind != ConnectFourTileKind_None) 
            return;
        
        set_tile(&state->board, tile.kind, tile.board_x, tile.board_y);
        Connect_Four_Winner winner = check_win(&state->board);
        if (winner != ConnectFourWinner_None) {
            // TODO(diego): Finish game and show menu.
            connect_four_game_restart(state);
            return;
        }
        
        switch_turns(state);
    }
}

internal b32
is_tile_empty(Connect_Four_Board *board, u32 tile_x, u32 tile_y) {
    assert(board);
    
    Connect_Four_Tile_Kind result;
    
    result = board->tiles[tile_x][tile_y].kind;
    
    return result == ConnectFourTileKind_None;
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
has_four_connected(Connect_Four_Board *board, Connect_Four_Tile_Kind kind, u32 start_x, u32 start_y, u32 advance_x, u32 advance_y) {
    if (start_x < 0 && start_x >= CONNECT_FOUR_X_COUNT) return false;
    if (start_y < 0 && start_y >= CONNECT_FOUR_Y_COUNT) return false;
    if (advance_x == 0 && advance_y == 0) return false;
    
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
    }
    
    return connected == 4;
}

internal Connect_Four_Winner
check_win(Connect_Four_Board *board) {
    
    Connect_Four_Winner result = ConnectFourWinner_None;
    
    u32 empty = CONNECT_FOUR_X_COUNT * CONNECT_FOUR_Y_COUNT;
    
    for (u32 x = 0; x < CONNECT_FOUR_X_COUNT; ++x) {
        for (u32 y = 0; y < CONNECT_FOUR_Y_COUNT; ++y) {
            if (has_four_connected(board, ConnectFourTileKind_Red, x, y, 1, 0) || has_four_connected(board, ConnectFourTileKind_Red, x, y, 0, 1) ||
                has_four_connected(board, ConnectFourTileKind_Red, x, y, 1, 1)) {
                result = ConnectFourWinner_Red;
                break;
            }
            if (has_four_connected(board, ConnectFourTileKind_Black, x, y, 1, 0) || has_four_connected(board, ConnectFourTileKind_Black, x, y, 0, 1) ||
                has_four_connected(board, ConnectFourTileKind_Black, x, y, 1, 1)) {
                result = ConnectFourWinner_Black;
                break;
            }
            if (board->tiles[x][y].kind != ConnectFourTileKind_None) {
                empty--;
            }
        }
    }
    
    if (empty == 0) {
        result = ConnectFourWinner_Tie;
    }
    
    return result;
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

// NOTE(diego): Horrible. Slow. Stupid.
internal void
best_move(Connect_Four_Board *board, Connect_Four_Tile_Kind player) {
    real32 best_score = -INFINITY;
    
    s32 best_tile_x = -1;
    s32 best_tile_y = -1;
    
    for (u32 x = 0; x < CONNECT_FOUR_X_COUNT; ++x) {
        for (u32 y = 0; y < CONNECT_FOUR_Y_COUNT; ++y) {
            if (is_tile_empty(board, x, y)) {
                set_tile(board, player, x, y);
                
                real32 score = minimax(board, 2, false);
                if (score > best_score) {
                    best_score = score;
                    best_tile_x = x;
                    best_tile_y = y;
                }
                set_tile(board, ConnectFourTileKind_None, x, y);
            }
        }
    }
    
    if (best_tile_x != -1 && best_tile_y != -1) {
        set_tile(board, player, best_tile_x, best_tile_y);
        Connect_Four_Winner winner = check_win(board);
        if (winner != ConnectFourWinner_None) {
            // TODO(diego): Finish game and show menu.
            connect_four_game_restart(board->state);
            return;
        }
        switch_turns(board->state);
    }
}

#define CONNECT_FOUR_TIE 0.f
#define CONNECT_FOUR_1 1.f
#define CONNECT_FOUR_2 4.f
#define CONNECT_FOUR_3 8.f
#define CONNECT_FOUR_4 10.f

#define CONNECT_FOUR_RED_1 CONNECT_FOUR_1
#define CONNECT_FOUR_RED_2 CONNECT_FOUR_2
#define CONNECT_FOUR_RED_3 CONNECT_FOUR_3
#define CONNECT_FOUR_RED_4 CONNECT_FOUR_4

#define CONNECT_FOUR_BLACK_1 -1.f
#define CONNECT_FOUR_BLACK_2 -4.f
#define CONNECT_FOUR_BLACK_3 -8.f
#define CONNECT_FOUR_BLACK_4 -10.f

// NOTE(diego): Horrible. Slow. Stupid.
internal real32
minimax(Connect_Four_Board *board, u32 depth, b32 maximizing_player) {
    
    real32 result = CONNECT_FOUR_TIE;
    if (depth == 0) {
        return result;
    }
    
    Connect_Four_Winner winner = check_win(board);
    switch (winner) {
        case ConnectFourWinner_Red: {
            result = maximizing_player ? CONNECT_FOUR_BLACK_4 : CONNECT_FOUR_RED_4;
        } break;
        case ConnectFourWinner_Black: {
            result = maximizing_player ? CONNECT_FOUR_RED_4 : CONNECT_FOUR_BLACK_4;
        } break;
        case ConnectFourWinner_Tie: {
            result = CONNECT_FOUR_TIE;
        } break;
        
        default: {
            // AI
            if (maximizing_player) {
                real32 best_score = -INFINITY;
                for (u32 x = 0; x < CONNECT_FOUR_X_COUNT; ++x) {
                    for (u32 y = 0; y < CONNECT_FOUR_Y_COUNT; ++y) {
                        if (is_tile_empty(board, x, y)) {
                            set_tile(board, board->state->ai_player, x, y);
                            best_score = max(minimax(board, depth - 1, false), best_score);
                            set_tile(board, ConnectFourTileKind_None, x, y);
                        }
                    }
                }
                result = best_score;
            } else {
                real32 best_score = INFINITY;
                for (u32 x = 0; x < CONNECT_FOUR_X_COUNT; ++x) {
                    for (u32 y = 0; y < CONNECT_FOUR_Y_COUNT; ++y) {
                        if (is_tile_empty(board, x, y)) {
                            set_tile(board, board->state->player, x, y);
                            best_score = min(minimax(board, depth - 1, true), best_score);
                            set_tile(board, ConnectFourTileKind_None, x, y);
                        }
                    }
                }
                result = best_score;
            }
            
        } break;
    }
    return result;
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
    real32 tile_size = max_size * 0.1f;
    
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
    Connect_Four_Tile ht = state->hovering_tile;
    if (ht.kind != ConnectFourTileKind_None) {
        immediate_circle_filled(ht.center, ht.radius, ht.color);
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
        
        // NOTE(diego): Not used.
        Memory_Index total_memory_size = kilobytes(16);
        state = (Connect_Four_State *) game_alloc(memory, total_memory_size);
        state->memory = memory;
        state->board.state = state;
        
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
