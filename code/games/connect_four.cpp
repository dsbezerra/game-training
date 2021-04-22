internal void
init_game(Connect_Four_State *state) {
    state->game_mode = GameMode_Playing;
    state->memory->game_mode = GameMode_Playing;
    state->player = ConnectFourTileKind_Red;
    state->ai_player = ConnectFourTileKind_Yellow;
    state->current_player = state->player;
    state->play_state = ConnectFourPlayState_RedTurn;
    state->computer_move_t_target = 0.f;
    state->computer_move_t = 0.f;
    state->move_t_target = 0.f;
    state->move_t = 0.f;
    
    platform_show_cursor(true);
    clear_board(&state->board);
}

internal void
update_game(Connect_Four_State *state, Game_Input *input) {
    b32 is_playing = state->play_state == ConnectFourPlayState_RedTurn || state->play_state == ConnectFourPlayState_YellowTurn;
    if (is_playing) {
        if (state->computer_move_t_target > .0f) {
            state->computer_move_t = move_towards(state->computer_move_t, state->computer_move_t_target, core.time_info.dt*2.f);
            if (state->computer_move_t >= state->computer_move_t_target) {
                state->computer_move_t = .0f;
                state->computer_move_t_target = 0.f;
                do_animated_move(&state->board, state->ai_player, state->move);
            }
            
        } else if (state->move_t_target > 0.f) {
            state->move_t = move_towards(state->move_t, state->move_t_target, core.time_info.dt * 3.f);
            if (state->move_t >= state->move_t_target) {
                state->move_t = .0f;
                state->move_t_target = 0.f;
                do_move(&state->board, state->current_player, state->move);
            }
        } else {
            if (state->current_player == state->player) {
                update_dragging_tile(state, input);
            } else if (state->current_player == state->ai_player) {
                best_move(&state->board, state->ai_player);
            }
        }
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
            do_animated_move(&state->board, state->player, x);
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
        state->play_state = ConnectFourPlayState_YellowTurn;
        state->current_player = ConnectFourTileKind_Yellow;
    } else if (state->play_state == ConnectFourPlayState_YellowTurn) {
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

internal void
do_move(Connect_Four_Board *board, Connect_Four_Tile_Kind player, s32 move) {
    if (is_valid_move(board, move)) {
        make_move(board, player, move);
        if (check_win(board, player)) {
            switch (player) {
                case ConnectFourTileKind_Red: {
                    board->state->play_state = ConnectFourPlayState_RedWin;
                } break;
                case ConnectFourTileKind_Yellow: {
                    board->state->play_state = ConnectFourPlayState_YellowWin;
                } break;
                default: break;
            }
        } else {
            switch_turns(board->state);
        }
    }
}

internal void
do_animated_move(Connect_Four_Board *board, Connect_Four_Tile_Kind player, s32 move) {
    Connect_Four_State *state = board->state;
    state->move = move;
    state->move_t = .0f;
    state->move_t_target = 1.f;
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
    
    Vector2 center = get_hud_player_position(dim, state->player);
    real32 tile_size = get_tile_size(dim);
    real32 radius = tile_size * .44f;
    
    real32 dx = state->mouse_position.x - center.x;
    real32 dy = state->mouse_position.y - center.y; 
    
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
            result = make_color(0xFFC90000);
        } break;
        
        case ConnectFourTileKind_Yellow: {
            result = make_color(0xFFFFE546);
        } break;
        
        case ConnectFourTileKind_None: {
            result = make_color(0xff2f3242);
        } break;
        
        invalid_default_case;
    }
    
    return result;
}

internal Vector2
get_hud_player_position(Vector2i dimensions, Connect_Four_Tile_Kind kind) {
    Vector2 result = {};
    
    switch (kind) {
        case ConnectFourTileKind_Red: {
            result = make_vector2(dimensions.width * 0.10f, dimensions.height * 0.9f);
        } break;
        
        case ConnectFourTileKind_Yellow: {
            result = make_vector2(dimensions.width * 0.90f, dimensions.height * 0.9f);
        } break;
        
        default: invalid_code_path;
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
    Connect_Four_State *state = board->state;
    
    Memory_Arena *arena = &state->moves_arena;
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
    
    state->move = best_moves[random_int_in_range(0, num_moves)];
    state->computer_move_t_target = 1.f;
    state->move_t_target = 0.f;
}

// NOTE(diego): We could use alpha-beta pruning to make it faster.
internal void
minimax(Connect_Four_Board *board, u32 depth, s32 *potential_moves) {
    if (depth == 0 || is_board_full(board)) return;
    
    Connect_Four_State *state = board->state;
    Memory_Arena *arena = &board->state->moves_arena;
    
    // NOTE(diego): This works only for AI player.
    Connect_Four_Tile_Kind tile_kind = state->ai_player;
    Connect_Four_Tile_Kind enemy_tile_kind = state->player;
    
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
        draw_board(state);
        b32 winner_exists = state->play_state == ConnectFourPlayState_RedWin || state->play_state == ConnectFourPlayState_YellowWin;
        if (winner_exists) {
            draw_win(state);
        }
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
    
    Vector4 board_color = make_color(0xFF2D86FA);
    Vector4 black = make_color(0xFF000000);
    Vector4 shadow = make_color(0x1a000000);
    
    immediate_begin();
    
    //
    // Background
    //
    immediate_quad(0.f, 0.f, (real32) dim.width, (real32) dim.height, make_color(0xff2f3242));
    
    //
    // Board
    //
    {
        Vector2 min = make_vector2(sx, sy);
        Vector2 max = make_vector2(min.x + x_width, sy + y_height);
        Vector2 offset = make_vector2(2.f, 10.f);
        immediate_quad(min + offset, max + offset, shadow);
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
    
    // Draw side tokens
    {
        // Draw player
        {
            Vector4 color = get_tile_color(state->player);
            Vector2 center = get_hud_player_position(dim, state->player);
            
            immediate_circle_filled(make_vector2(center.x + 2.f, center.y + 2.f), radius*1.05f, shadow);
            immediate_circle_filled(center, radius, color);
        }
        
        // Draw AI player
        {
            Vector4 color = get_tile_color(state->ai_player);
            Vector2 center = get_hud_player_position(dim, state->ai_player);
            
            immediate_circle_filled(make_vector2(center.x + 2.f, center.y + 2.f), radius*1.05f, shadow);
            immediate_circle_filled(center, radius, color);
        }
        
    }
    
    immediate_flush();
    
    //
    // Hovering
    //
    
    immediate_begin();
    
    Connect_Four_Tile ht = state->dragging_tile;
    if (ht.kind != ConnectFourTileKind_None) {
        
        //
        // Draw text
        //
        
        char *text = "Drag over the board to place";
        real32 text_width = get_text_width(&state->assets.drag_over_font, text);
        
        real32 text_x = center_horizontally((real32) dim.width, text_width);
        real32 text_y = center_vertically(sy, 0.f);
        
        Vector4 white = make_color(0xffffffff);
        draw_text(text_x+1.f, text_y+2.f, (u8 *) text, &state->assets.drag_over_font, black);
        draw_text(text_x, text_y, (u8 *) text, &state->assets.drag_over_font, white);
        
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
        } else {
            immediate_circle_filled(make_vector2(dragging.x + 2.f, dragging.y + 2.f), radius*1.05f, shadow);
        }
        
        immediate_circle_filled(dragging, radius, ht.color);
    }
    
    //
    // Draw AI animated move
    //
    
    if (state->computer_move_t_target > 0.f) {
        Vector4 color = get_tile_color(state->ai_player);
        Vector2 start_center = get_hud_player_position(dim, state->ai_player);
        Vector2 final_center = make_vector2(sx + tile_size * state->move + tile_size*.5f, sy * .5f);
        
        Vector2 distance = final_center - start_center;
        
        Vector2 t_center = distance * smooth_stop2(state->computer_move_t);
        
        immediate_circle_filled(start_center + t_center, radius, color);
    }
    
    //
    // Draw animated drop
    //
    
    if (state->move_t_target > 0.f) {
        Connect_Four_Move move = get_lowest_empty_move(&state->board, state->move);
        
        real32 half_tile_size = tile_size*.5f;
        Vector4 color = get_tile_color(state->current_player);
        
        Vector2 start_center = {};
        if (state->current_player == state->ai_player) {
            start_center.x = sx + tile_size * move.x + half_tile_size;
            start_center.y = sy * .5f;
        } else {
            start_center.x = (real32) state->mouse_position.x;
            start_center.y = (real32) state->mouse_position.y;
        }
        
        Vector2 final_center = make_vector2(sx + tile_size * move.x + half_tile_size, sy + tile_size * move.y + half_tile_size);
        
        Vector2 distance = final_center - start_center;
        Vector2 t_center = distance * smooth_stop5(state->move_t);
        
        immediate_circle_filled(start_center + t_center, radius, color);
    }
    
#define TEST_SMOOTH_FUNCTIONS 0
#if TEST_SMOOTH_FUNCTIONS
    {
        
        real32 offset = 100.f;
        
        u32 samples = 200;
        real32 size = samples * .01f;
        
        real32 y = dim.height * .5f;
        real32 height = 200.f;
        
        for (u32 x = 0; x < samples; x++) {
            // How much we need to move Y for current X;
            real32 t = smooth_stop2((real32) x / (real32) samples);
            
            real32 point_x = (real32) x;
            real32 point_y = y - t * height;
            
            Vector2 min = make_vector2(offset + point_x, point_y);
            Vector2 max = make_vector2(min.x + size, min.y + size);
            immediate_quad(min, max, make_color(0xffff0000));
        }
        
        real32 wh = height;
        
        // Bottom
        {
            Vector2 min = make_vector2(offset, y);
            Vector2 max = make_vector2(min.x + wh, min.y + 1.f);
            immediate_quad(min, max, make_color(0x1affffff));
        }
        
        // Top
        {
            Vector2 min = make_vector2(offset, y - wh);
            Vector2 max = make_vector2(min.x + wh, min.y + 1.f);
            immediate_quad(min, max, make_color(0x1affffff));
        }
        
        // Left
        {
            Vector2 min = make_vector2(offset, y - wh);
            Vector2 max = make_vector2(min.x + 1.f, min.y + wh);
            immediate_quad(min, max, make_color(0x1affffff));
        }
        
        
        // Right
        {
            Vector2 min = make_vector2(offset + wh, y - wh + 1.f);
            Vector2 max = make_vector2(min.x + 1.f, min.y + wh);
            immediate_quad(min, max, make_color(0x1affffff));
        }
        
        // Circle
        local_persist real32 curve_t = -0.2f;
        {
            real32 tt = clampf(0.f, curve_t, 1.f);
            real32 xx = tt * height;
            real32 yy = smooth_stop2(tt) * height;
            
            Vector2 center = make_vector2(offset + xx, y - yy);
            radius = height * 0.04f;
            immediate_circle_filled(center, radius, make_color(0xffffff00));
        }
        curve_t += core.time_info.dt;
        if (curve_t > 1.2f) {
            curve_t = -.2f;
        }
    }
#endif
    
    immediate_flush();
}

internal void
draw_win(Connect_Four_State *state) {
    Vector2i dim = state->memory->window_dimensions;
    
    Vector4 text_color = make_color(0xffffffff);
    Vector4 shadow_color = make_color(0xaa000000);
    
    real32 center_x = (dim.width / 2.f);
    real32 center_y = (dim.height / 2.f);
    
    //
    // Overlay
    //
    
    immediate_begin();
    immediate_quad(make_vector2(0.f, 0.f), make_vector2((real32) dim.width, (real32) dim.height), shadow_color);
    immediate_flush();
    
    // WINNER
    {
        char *winners[2] = {"Red's Win", "Yellow's Win"};
        
        char *actual_text = 0;
        switch (state->play_state) {
            case ConnectFourPlayState_RedWin: actual_text = winners[0]; break;
            case ConnectFourPlayState_YellowWin: actual_text = winners[1]; break;
            default: break;
        }
        
        if (actual_text) {
            {
                real32 text_width = get_text_width(&state->assets.finish_font, actual_text);
                real32 text_x = center_x - text_width / 2.f;
                real32 text_y = (dim.height * .4f) - state->assets.finish_font.line_height * .5f;
                draw_text(text_x, text_y, (u8 *) actual_text, &state->assets.finish_font, text_color);
            }
            
            // Press ESC to retry
            {
                int line_count;
                char *text = "Press ESC to retry";
                real32 text_width = get_text_width(&state->assets.esc_font, text, &line_count);
                real32 text_x = center_x - text_width / 2.f;
                real32 text_y = (dim.height * .6f) - state->assets.esc_font.line_height * .5f;
                
                draw_text(text_x, text_y, (u8 *) text, &state->assets.esc_font, text_color);
            }
        }
    }
    
    
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
        
        Connect_Four_Assets assets = {};
        assets.drag_over_font = load_font("./data/fonts/Inconsolata-Regular.ttf", 42.f);
        assets.finish_font = load_font("./data/fonts/Inconsolata-Regular.ttf", 42.f);
        assets.esc_font = load_font("./data/fonts/Inconsolata-Regular.ttf", 26.f);
        
        state = (Connect_Four_State *) game_alloc(memory, total_memory_size);
        state->memory = memory;
        state->board.state = state;
        state->assets = assets;
        
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
