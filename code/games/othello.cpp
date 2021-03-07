internal void
init_game(Othello_State *state) {
    
    state->game_mode = GameMode_Playing;
    state->board.state = state;
    
    set_play_state(state, OthelloPlayState_BlackTurn);
    
    move_list_clear(state);
    
    reset_board(&state->board);
    
    platform_show_cursor(true);
}

internal void
update_game(Othello_State *state, Game_Input *input) {
    b32 is_turn = state->play_state == OthelloPlayState_BlackTurn || state->play_state == OthelloPlayState_WhiteTurn;
    b32 won = state->play_state == OthelloPlayState_BlackWin || state->play_state == OthelloPlayState_WhiteWin;
    if (is_turn) {
        update_hovering_tile(state);
        if (pressed(Button_Mouse1)) {
            make_move(state);
        }
    } else if (won) {
        if (pressed(Button_Enter)) {
            othello_game_restart(state);
        }
    }
}

internal void
update_hovering_tile(Othello_State *state) {
    Vector2i mouse_position;
    platform_get_cursor_position(&mouse_position);
    
    state->hovering_tile.kind = OthelloTileKind_Hover; // Useless for now.
    state->hovering_tile.x = -1;
    state->hovering_tile.y = -1;
    
    real32 tile_size = get_tile_size(state);
    Vector2 start = get_start_xy(state);
    
    real32 max_x = start.x + tile_size * OTHELLO_BOARD_COUNT;
    real32 max_y = start.y + tile_size * OTHELLO_BOARD_COUNT;
    
    if (mouse_position.x < start.x || mouse_position.x > max_x) return;
    if (mouse_position.y < start.y || mouse_position.y > max_y) return;
    
    s32 x = (s32) ((mouse_position.x - start.x) / tile_size);
    s32 y = (s32) ((mouse_position.y - start.y) / tile_size);
    
    assert(x >= 0 && x < OTHELLO_BOARD_COUNT);
    assert(y >= 0 && y < OTHELLO_BOARD_COUNT);
    
    state->hovering_tile.x = x;
    state->hovering_tile.y = y;
}

internal void
draw_board(Othello_State *state) {
    
    immediate_begin();
    
    Vector2 start = get_start_xy(state);
    
    // Board
    Vector2i dim = state->dimensions;
    real32 height = dim.height * .9f;
    real32 tile_size = height / (real32) OTHELLO_BOARD_COUNT;
    
    real32 width  = tile_size * OTHELLO_BOARD_COUNT;
    
    {
        Vector2 min = make_vector2(start.x, start.y);
        Vector2 max = make_vector2(min.x + width, min.y + height);
        Vector4 color = make_color(0xff009a00);
        
        immediate_quad(min, max, color);
    }
    
    // Lines
    {
        
        Vector4 line_color = make_color(0xff000000);
        u32 num_lines = OTHELLO_BOARD_COUNT + 1;
        
        real32 x_offset = .0f;
        real32 y_offset = .0f;
        
        real32 line_thickness = 1.f;
        
        for (u32 line_index = 0; line_index < num_lines; ++line_index) {
            
            // Horizontal
            Vector2 h_min = make_vector2(start.x, start.y + y_offset);
            Vector2 h_max = make_vector2(h_min.x + width, h_min.y + line_thickness);
            immediate_quad(h_min, h_max, line_color);
            
            // Vertical
            Vector2 v_min = make_vector2(start.x + x_offset, start.y);
            Vector2 v_max = make_vector2(v_min.x + line_thickness, v_min.y + height);
            immediate_quad(v_min, v_max, line_color);
            
            x_offset += tile_size;
            y_offset += tile_size;
        }
    }
    
    Vector4 white = make_color(0xffffffff);
    
    // Tiles
    {
        Vector4 black = make_color(0xff000000);
        
        Othello_Board *b = &state->board;
        
        real32 tile_margin = tile_size * 0.1f;
        
        for (u32 tile_x = 0; tile_x < OTHELLO_BOARD_COUNT; ++tile_x) {
            for (u32 tile_y = 0; tile_y < OTHELLO_BOARD_COUNT; ++tile_y) {
                Othello_Tile tile = b->tiles[tile_x][tile_y];
                if (tile.kind == OthelloTileKind_None) continue;
                
                Vector4 color;
                if (tile.kind == OthelloTileKind_Black)
                    color = black;
                else
                    color = white;
                
                
                real32 sx = start.x + tile_x * tile_size;
                real32 sy = start.y + tile_y * tile_size;
                
                // Move to center of tile size
                Vector2 center = make_vector2(sx + tile_size * .5f, sy + tile_size * .5f);
                
                real32 radius = tile_size * 0.4f;
                immediate_circle_filled(center, radius, color);
            }
        }
    }
    
#define DRAW_MOVE_LIST 1
#if DRAW_MOVE_LIST
    // Draw move list
    {
        Vector4 opaque;
        Vector4 transparent;
        if (state->current_player == OthelloTileKind_Black) {
            opaque = make_color(0xff000000);
            transparent = make_color(0x00000000);
        } else {
            opaque = make_color(0xffffffff);
            transparent = make_color(0x00ffffff);
        }
        
        Vector4 color = lerp_color(transparent, 0.2f, opaque);
        
        Othello_Move *move = state->move_list.first;
        while (move) {
            
            real32 sx = start.x + move->x * tile_size;
            real32 sy = start.y + move->y * tile_size;
            
            // Move to center of tile size
            Vector2 center = make_vector2(sx + tile_size * .5f, sy + tile_size * .5f);
            
            real32 radius = tile_size * 0.4f;
            immediate_circle_filled(center, radius, color);
            
            move = move->next;
        }
    }
#endif
    
    b32 is_turn = state->play_state == OthelloPlayState_BlackTurn || state->play_state == OthelloPlayState_WhiteTurn;
    b32 hovering = state->hovering_tile.x != -1 && state->hovering_tile.y != -1;
    if (hovering && is_turn) {
        u32 tile_x = (u32) state->hovering_tile.x;
        u32 tile_y = (u32) state->hovering_tile.y;
        
        Vector2 min = make_vector2(start.x + tile_x * tile_size, start.y + tile_y * tile_size);
        Vector2 max = make_vector2(min.x + tile_size, min.y + tile_size);
        Vector4 color = make_color(0xaafcf45a);
        immediate_quad(min, max, color);
    }
    
    immediate_flush();
    
    // Draw board letters and numbers
    {
        real32 hlh = state->assets.board_font.line_height/2.f;
        real32 half_tile_size = tile_size * .5f;
        
        immediate_begin();
        for (u32 tile_x = 0; tile_x < OTHELLO_BOARD_COUNT; ++tile_x) {
            char letter = get_move_letter(tile_x);
            real32 sx = start.x + tile_x * tile_size + half_tile_size;
            
            real32 cw = get_char_width(&state->assets.board_font, letter) * .5f;
            immediate_char(sx - cw, start.y - 10.f, letter, &state->assets.board_font, white);
        }
        for (u32 tile_y = 0; tile_y < OTHELLO_BOARD_COUNT; ++tile_y) {
            char number = get_move_number(tile_y);
            real32 sy = start.y + tile_y * tile_size + half_tile_size + hlh * .5f;
            immediate_char(start.x - 20.f, sy, number, &state->assets.board_font, white);
        }
        
        // Draw hovering
        if (hovering && is_turn) {
            u32 tile_x = (u32) state->hovering_tile.x;
            u32 tile_y = (u32) state->hovering_tile.y;
            
            char letter = get_move_letter(tile_x);
            char number = get_move_number(tile_y);
            
            Loaded_Font *f = &state->assets.board_font;
            
            char move_pair[3] = {letter, number, '\0'};
            int line_count;
            real32 line_width = get_text_width(f, move_pair, &line_count);
            real32 half_line_width = line_width * .5f;
            
            immediate_text(start.x + tile_x * tile_size + half_tile_size - half_line_width, start.y + tile_y * tile_size + half_tile_size + hlh * .5f, (u8 *) move_pair, f, white, -5.f);
        }
        
        immediate_flush();
    }
    
}

internal void
draw_game_view(Othello_State *state) {
    
    game_frame_begin(state->dimensions.width, state->dimensions.height);
    
    if (state->game_mode == GameMode_Playing) {
        immediate_begin();
        immediate_quad(0.f, 0.f, (real32) state->dimensions.width, (real32) state->dimensions.height, make_color(0xff2f3242));
        immediate_flush();
        
        draw_board(state);
        draw_hud(state);
    } else {
        draw_menu(OTHELLO_TITLE, state->dimensions, state->game_mode, state->menu_selected_item, state->quit_was_selected);
    }
}

internal void
draw_hud(Othello_State *state) {
    
    b32 is_turn = state->play_state == OthelloPlayState_BlackTurn || state->play_state == OthelloPlayState_WhiteTurn;
    
    b32 won = state->play_state == OthelloPlayState_BlackWin || state->play_state == OthelloPlayState_WhiteWin;
    
    Vector2i dim = state->dimensions;
    
    if (is_turn) {
        char *turn_text[] = {"White's Turn", "Black's Turn", '\0'};
        
        s32 i = -1;
        if (state->play_state == OthelloPlayState_WhiteTurn) i = 0;
        else if (state->play_state == OthelloPlayState_BlackTurn) i = 1;
        
        if (i >= 0) {
            real32 x = dim.width  * .5f;
            real32 y = dim.height * 0.99f;
            
            real32 tw = get_text_width(&state->assets.turn_font, turn_text[i]);
            
            Vector4 color = make_color(0xffffffff);
            draw_text(x - tw / 2.f, y, (u8 *) turn_text[i], &state->assets.turn_font, color);
        }
    } else if (won) {
        char *won_text[] = {"White Won", "Black Won", '\0'};
        
        s32 i = -1;
        if (state->play_state == OthelloPlayState_WhiteWin) i = 0;
        else if (state->play_state == OthelloPlayState_BlackWin) i = 1;
        
        immediate_begin();
        immediate_quad(0.f, 0.f, (real32) dim.width, (real32) dim.height, make_color(0x44000000));
        immediate_flush();
        
        if (i >= 0) {
            real32 x = dim.width  * .5f;
            real32 y = dim.height * 0.99f;
            
            real32 tw = get_text_width(&state->assets.turn_font, won_text[i]);
            
            Vector4 color = make_color(0xffffffff);
            draw_text(x - tw / 2.f, y, (u8 *) won_text[i], &state->assets.turn_font, color);
        }
    }
}

internal void
reset_board(Othello_Board *board) {
    clear_board(board);
    
#define OTHELLO_TEST 1
    
#define OTHELLO_TEST_1 0
#define OTHELLO_TEST_2 1
#define OTHELLO_TEST_3 2
    
#if OTHELLO_TEST
#define OTHELLO_TEST_ENABLED OTHELLO_TEST_3 // Change here to test.
#endif
    
#define OTHELLO_TEST_1_ENABLED OTHELLO_TEST_ENABLED == OTHELLO_TEST_1
#define OTHELLO_TEST_2_ENABLED OTHELLO_TEST_ENABLED == OTHELLO_TEST_2
#define OTHELLO_TEST_3_ENABLED OTHELLO_TEST_ENABLED == OTHELLO_TEST_3
    
    // Default initial board
#ifndef OTHELLO_TEST_ENABLED
    set_tile(board, OthelloTileKind_White, "D4");
    set_tile(board, OthelloTileKind_Black, "E4");
    set_tile(board, OthelloTileKind_Black, "D5");
    set_tile(board, OthelloTileKind_White, "E5");
#elif OTHELLO_TEST_1_ENABLED
    set_tile(board, OthelloTileKind_Black, "E4");
    set_tile(board, OthelloTileKind_Black, "E5");
    set_tile(board, OthelloTileKind_Black, "E6");
    set_tile(board, OthelloTileKind_Black, "D5");
    set_tile(board, OthelloTileKind_Black, "F5");
    set_tile(board, OthelloTileKind_White, "D4");
    set_tile(board, OthelloTileKind_White, "F6");
    set_play_state(board->state, OthelloPlayState_WhiteTurn);
#elif OTHELLO_TEST_2_ENABLED
    set_tile(board, OthelloTileKind_White, "E2");
    set_tile(board, OthelloTileKind_White, "D4");
    set_tile(board, OthelloTileKind_White, "F4");
    set_tile(board, OthelloTileKind_White, "C5");
    set_tile(board, OthelloTileKind_White, "D5");
    set_tile(board, OthelloTileKind_White, "E5");
    set_tile(board, OthelloTileKind_White, "F5");
    set_tile(board, OthelloTileKind_White, "G5");
    set_tile(board, OthelloTileKind_White, "D6");
    set_tile(board, OthelloTileKind_White, "E6");
    set_tile(board, OthelloTileKind_White, "F6");
    set_tile(board, OthelloTileKind_White, "D7");
    
    set_tile(board, OthelloTileKind_Black, "C3");
    set_tile(board, OthelloTileKind_Black, "D3");
    set_tile(board, OthelloTileKind_Black, "E3");
    set_tile(board, OthelloTileKind_Black, "F3");
    set_tile(board, OthelloTileKind_Black, "C4");
    set_tile(board, OthelloTileKind_Black, "E4");
    set_tile(board, OthelloTileKind_Black, "E3");
    set_tile(board, OthelloTileKind_Black, "E7");
    set_tile(board, OthelloTileKind_Black, "G6");
#elif OTHELLO_TEST_3_ENABLED
    set_tile(board, OthelloTileKind_White, "B1");
    set_tile(board, OthelloTileKind_White, "C1");
    set_tile(board, OthelloTileKind_White, "D1");
    set_tile(board, OthelloTileKind_White, "E1");
    set_tile(board, OthelloTileKind_White, "F1");
    set_tile(board, OthelloTileKind_White, "G1");
    set_tile(board, OthelloTileKind_White, "A2");
    set_tile(board, OthelloTileKind_White, "B2");
    set_tile(board, OthelloTileKind_White, "C2");
    set_tile(board, OthelloTileKind_White, "D2");
    set_tile(board, OthelloTileKind_White, "F2");
    set_tile(board, OthelloTileKind_White, "H2");
    set_tile(board, OthelloTileKind_White, "A3");
    set_tile(board, OthelloTileKind_White, "B3");
    set_tile(board, OthelloTileKind_White, "E3");
    set_tile(board, OthelloTileKind_White, "G3");
    set_tile(board, OthelloTileKind_White, "H3");
    set_tile(board, OthelloTileKind_White, "A4");
    set_tile(board, OthelloTileKind_White, "B4");
    set_tile(board, OthelloTileKind_White, "C4");
    set_tile(board, OthelloTileKind_White, "D4");
    set_tile(board, OthelloTileKind_White, "F4");
    set_tile(board, OthelloTileKind_White, "G4");
    set_tile(board, OthelloTileKind_White, "H4");
    set_tile(board, OthelloTileKind_White, "A5");
    set_tile(board, OthelloTileKind_White, "B5");
    set_tile(board, OthelloTileKind_White, "C5");
    set_tile(board, OthelloTileKind_White, "F5");
    set_tile(board, OthelloTileKind_White, "G5");
    set_tile(board, OthelloTileKind_White, "H5");
    set_tile(board, OthelloTileKind_White, "A6");
    set_tile(board, OthelloTileKind_White, "B6");
    set_tile(board, OthelloTileKind_White, "D6");
    set_tile(board, OthelloTileKind_White, "E6");
    set_tile(board, OthelloTileKind_White, "F6");
    set_tile(board, OthelloTileKind_White, "G6");
    set_tile(board, OthelloTileKind_White, "H6");
    set_tile(board, OthelloTileKind_White, "A7");
    set_tile(board, OthelloTileKind_White, "E7");
    set_tile(board, OthelloTileKind_White, "F8");
    
    set_tile(board, OthelloTileKind_Black, "H1");
    set_tile(board, OthelloTileKind_Black, "E2");
    set_tile(board, OthelloTileKind_Black, "G2");
    set_tile(board, OthelloTileKind_Black, "C3");
    set_tile(board, OthelloTileKind_Black, "D3");
    set_tile(board, OthelloTileKind_Black, "F3");
    set_tile(board, OthelloTileKind_Black, "E4");
    set_tile(board, OthelloTileKind_Black, "D5");
    set_tile(board, OthelloTileKind_Black, "E5");
    set_tile(board, OthelloTileKind_Black, "C6");
    set_tile(board, OthelloTileKind_Black, "B7");
    set_tile(board, OthelloTileKind_Black, "C7");
    set_tile(board, OthelloTileKind_Black, "A8");
    set_tile(board, OthelloTileKind_Black, "B8");
    set_tile(board, OthelloTileKind_Black, "C8");
#endif
    
    move_list_update(board->state);
}

internal void
clear_board(Othello_Board *board) {
    for (u32 tile_x = 0; tile_x < OTHELLO_BOARD_COUNT; ++tile_x) {
        for (u32 tile_y = 0; tile_y < OTHELLO_BOARD_COUNT; ++tile_y) {
            Othello_Tile empty = {};
            empty.x = tile_x;
            empty.y = tile_y;
            empty.letter = get_move_letter(tile_x);
            empty.number = get_move_number(tile_y);
            empty.coordinate[0] = empty.letter; 
            empty.coordinate[1] = empty.number; 
            empty.coordinate[2] = '\0'; 
            
            board->tiles[tile_x][tile_y] = empty;
        }
    }
}

internal Othello_Tile_Kind
check_for_win(Othello_State *state) {
    assert(state->play_state == OthelloPlayState_WhiteTurn || state->play_state == OthelloPlayState_BlackTurn);
    
    Othello_Tile_Kind result = {};
    
    Othello_Board *board = &state->board;
    if (state->move_list.size == 0) {
        u32 black = 0;
        u32 white = 0;
        for (u32 tile_x = 0; tile_x < OTHELLO_BOARD_COUNT; ++tile_x) {
            for (u32 tile_y = 0; tile_y < OTHELLO_BOARD_COUNT; ++tile_y) {
                Othello_Tile tile = board->tiles[tile_x][tile_y];
                if (tile.kind == OthelloTileKind_Black) black++;
                if (tile.kind == OthelloTileKind_White) white++;
            }
        }
        
        if (black > white) result = OthelloTileKind_Black;
        if (white > black) result = OthelloTileKind_White;
    }
    
    return result;
}

internal void
eat_tiles_for_move(Othello_Board *board, Othello_Move *move) {
    if (!move) return;
    
    // Check tiles in between
    s32 diff_x = 0;
    if (move->origin.x < move->x)      diff_x = -1;
    else if (move->origin.x > move->x) diff_x =  1;
    
    s32 diff_y = 0;
    if (move->origin.y < move->y)      diff_y = -1;
    else if (move->origin.y > move->y) diff_y =  1;
    
    s32 set_x = move->x;
    s32 set_y = move->y;
    
    while (1) {
        set_tile(board, board->state->current_player, set_x, set_y);
        set_x += diff_x;
        set_y += diff_y;
        
        if (set_x == move->origin.x && set_y == move->origin.y) break;
    }
    
    if (diff_x != 0) {
        eat_tiles_to_side_of_move(board, move, OthelloSide_Up);
        eat_tiles_to_side_of_move(board, move, OthelloSide_Down);
    }
    
    if (diff_y != 0) {
        eat_tiles_to_side_of_move(board, move, OthelloSide_Right);
        eat_tiles_to_side_of_move(board, move, OthelloSide_Left);
    }
    
    eat_tiles_to_side_of_move(board, move, OthelloSide_UpRight);
    eat_tiles_to_side_of_move(board, move, OthelloSide_UpLeft);
    eat_tiles_to_side_of_move(board, move, OthelloSide_DownRight);
    eat_tiles_to_side_of_move(board, move, OthelloSide_DownLeft);
}

internal void
eat_tiles_to_side_of_move(Othello_Board *board, Othello_Move *move, Othello_Side side) {
    Othello_Tile *tile = find_tile_to_side_of_move(board, move, side);
    if (!tile) return;
    
    s32 diff_x = 0;
    if (side == OthelloSide_Left || side == OthelloSide_UpLeft || side == OthelloSide_DownLeft) {
        diff_x = -1;
    } else if (side == OthelloSide_Right || side == OthelloSide_UpRight || side == OthelloSide_DownRight) {
        diff_x = 1;
    }
    
    s32 diff_y = 0;
    if (side == OthelloSide_Up || side == OthelloSide_UpLeft || side == OthelloSide_UpRight) {
        diff_y = -1;
    } else if (side == OthelloSide_Down || side == OthelloSide_DownLeft || side == OthelloSide_DownRight) {
        diff_y = 1;
    }
    
    s32 x = move->x;
    s32 y = move->y;
    
    while (1) {
        x += diff_x;
        y += diff_y;
        if (tile->x == x && tile->y == y) {
            break;
        }
        set_tile(board, board->state->current_player, x, y);
    }
}

internal Othello_Tile *
find_tile_to_side_of_move(Othello_Board *board, Othello_Move *move, Othello_Side side) {
    Othello_Tile * result = 0;
    
    s32 diff_x = 0;
    s32 diff_y = 0;
    
    if (side == OthelloSide_Left || side == OthelloSide_UpLeft || side == OthelloSide_DownLeft)
        diff_x = -1;
    
    if (side == OthelloSide_Right || side == OthelloSide_UpRight || side == OthelloSide_DownRight)
        diff_x =  1;
    
    if (side == OthelloSide_Up || side == OthelloSide_UpLeft || side == OthelloSide_UpRight)
        diff_y = -1;
    
    if (side == OthelloSide_Down || side == OthelloSide_DownLeft || side == OthelloSide_DownRight)
        diff_y =  1;
    
    s32 x = move->x;
    s32 y = move->y;
    
    while (1) {
        x += diff_x;
        y += diff_y;
        if (x < 0 || y < 0 || x >= OTHELLO_BOARD_COUNT || y >= OTHELLO_BOARD_COUNT) {
            break;
        }
        
        Othello_Tile *tile = &board->tiles[x][y];
        if (tile->kind == OthelloTileKind_None) {
            break;
        }
        
        if (tile->kind == move->origin.kind) {
            result = tile;
            break;
        }
    }
    
    return result;
}

internal void
move_list_clear(Othello_State *state) {
    reset_arena(&state->move_arena);
    
    state->move_list.first = 0;
    state->move_list.head = 0;
    state->move_list.size = 0;
}

internal Othello_Move *
move_list_find(Othello_State *state, Othello_Move move) {
    Othello_Move *result = 0;
    
    Othello_Move_List *list = &state->move_list;
    if (list->size != 0) {
        Othello_Move *m = list->first;
        while (m) {
            if (m->x == move.x && m->y == move.y) {
                result = m;
                break;
            }
            m = m->next;
        }
    }
    
    return result;
}

internal void
move_list_add(Othello_State *state, Othello_Move move) {
    Othello_Move_List *list = &state->move_list;
    
    Othello_Move *result = push_struct(&state->move_arena, Othello_Move);
    
    result->x = move.x;
    result->y = move.y;
    result->origin = move.origin;
    result->next = 0;
    
    if (!list->first) {
        list->first = result;
    } else {
        list->head->next = result;
    }
    
    list->head = result;
    list->size++;
}

internal void
move_list_add_if_unique(Othello_State *state, Othello_Move move) {
    Othello_Move *found = move_list_find(state, move);
    if (found) return;
    
    move_list_add(state, move);
}

internal void
move_list_update(Othello_State *state) {
    move_list_clear(state);
    
    Othello_Board *board = &state->board;
    
    for (u32 tile_x = 0; tile_x < OTHELLO_BOARD_COUNT; ++tile_x) {
        for (u32 tile_y = 0; tile_y < OTHELLO_BOARD_COUNT; ++tile_y) {
            Othello_Tile *tile = &board->tiles[tile_x][tile_y];
            
            b32 is_player_tile = tile->kind == state->current_player;
            if (is_player_tile) {
                move_list_check(state, tile, -1,  0); // Check left
                move_list_check(state, tile,  1,  0); // Check right
                move_list_check(state, tile,  0, -1); // Check up
                move_list_check(state, tile,  0,  1); // Check down
                move_list_check(state, tile, -1, -1); // Check up-right
                move_list_check(state, tile,  1, -1); // Check up-left
                move_list_check(state, tile, -1,  1); // Check down-right
                move_list_check(state, tile,  1,  1); // Check down-left
            }
        }
    }
}

internal void
move_list_check(Othello_State *state, Othello_Tile *tile, s32 x, s32 y) {
    Othello_Tile direction = {};
    direction.kind = OthelloTileKind_FindDirection;
    direction.x = x;
    direction.y = y;
    
    Othello_Tile *result = find_valid_move_for_tile(&state->board, tile, direction);
    if (result) {
        Othello_Tile origin_tile = {};
        origin_tile.kind = tile->kind;
        origin_tile.x = tile->x;
        origin_tile.y = tile->y;
        
        Othello_Move move = {};
        move.x = result->x;
        move.y = result->y;
        move.origin = origin_tile;
        
        move_list_add_if_unique(state, move);
    }
}

internal Othello_Move *
move_list_valid(Othello_State *state, Othello_Tile *tile) {
    if (!tile) return 0;
    
    Othello_Move move = {};
    move.x = tile->x;
    move.y = tile->y;
    
    return move_list_find(state, move);
}

internal void
make_move(Othello_State *state) {
    assert(state->play_state == OthelloPlayState_BlackTurn || state->play_state == OthelloPlayState_WhiteTurn);
    
    Othello_Move *move = move_list_valid(state, &state->hovering_tile);
    if (!move) return;
    
    eat_tiles_for_move(&state->board, move);
    
    switch_turns(state);
    
    Othello_Tile_Kind won = check_for_win(state);
    switch (won) {
        case OthelloTileKind_Black: set_play_state(state, OthelloPlayState_BlackWin); break;
        case OthelloTileKind_White: set_play_state(state, OthelloPlayState_WhiteWin); break;
        default: break;
    }
}

internal b32
is_empty(Othello_Board *board, u32 tile_x, u32 tile_y) {
    b32 result = false;
    
    assert(tile_x >= 0 && tile_x < OTHELLO_BOARD_COUNT);
    assert(tile_y >= 0 && tile_y < OTHELLO_BOARD_COUNT);
    
    Othello_Tile tile_at_xy = board->tiles[tile_x][tile_y];
    
    result = tile_at_xy.kind == OthelloTileKind_None;
    
    return result;
}

internal char
get_move_letter(u32 tile_x) {
    assert(tile_x >= 0 && tile_x < OTHELLO_BOARD_COUNT);
    
    char result = (char) ('A' + tile_x);
    
    return result;
}

internal char
get_move_number(u32 tile_y) {
    assert(tile_y >= 0 && tile_y < OTHELLO_BOARD_COUNT);
    
    char result = (char) ('1' + tile_y);
    
    return result;
}

internal Othello_Coordinate
get_coordinate(char *coordinate) {
    assert(string_length(coordinate) == 2);
    
    Othello_Coordinate result = {};
    
    char letter = coordinate[0];
    char number = coordinate[1];
    
    result.x = (u32) (letter - 'A');
    result.y = (u32) (number - '1');
    
    assert(result.x >= 0 && result.x < OTHELLO_BOARD_COUNT);
    assert(result.y >= 0 && result.y < OTHELLO_BOARD_COUNT);
    
    return result;
}

internal Vector2
get_start_xy(Othello_State *state) {
    
    Vector2 result = make_vector2(0.f, 0.f);
    
    Vector2i dim = state->dimensions;
    
    real32 height = dim.height * .9f;
    real32 tile_size = height / (real32) OTHELLO_BOARD_COUNT;
    
    real32 width  = tile_size * OTHELLO_BOARD_COUNT;
    real32 remaining_wspace = dim.width - width;
    real32 remaining_hspace = dim.height - height;
    
    result.x = remaining_wspace * .5f;
    result.y = remaining_hspace * .5f;
    
    return result;
}

internal real32
get_tile_size(Othello_State *state) {
    real32 result = .0f;
    
    real32 height = state->dimensions.height * .9f;
    
    result = height / (real32) OTHELLO_BOARD_COUNT;
    
    return result;
}

internal void
switch_turns(Othello_State *state) {
    if (state->play_state == OthelloPlayState_BlackTurn)
        set_play_state(state, OthelloPlayState_WhiteTurn);
    else if (state->play_state == OthelloPlayState_WhiteTurn)
        set_play_state(state, OthelloPlayState_BlackTurn);
    
    move_list_update(state);
}

internal void
set_play_state(Othello_State *state, Othello_Play_State new_state) {
    state->play_state = new_state;
    
    switch (new_state) {
        case OthelloPlayState_BlackTurn: state->current_player = OthelloTileKind_Black; break;
        case OthelloPlayState_WhiteTurn: state->current_player = OthelloTileKind_White; break;
        default: break;
    }
}

internal void
set_tile(Othello_Board *board, Othello_Tile_Kind kind, u32 tile_x, u32 tile_y) {
    // NOTE(diego): Assumes tile_x and tile_y are within bounds.
    Othello_Tile *tile = &board->tiles[tile_x][tile_y];
    
    tile->kind = kind;
    tile->x = tile_x;
    tile->y = tile_y;
}

internal void
set_tile(Othello_Board *board, Othello_Tile_Kind kind, char *coordinate) {
    Othello_Coordinate coord = get_coordinate(coordinate);
    set_tile(board, kind, coord.x, coord.y);
}

internal Othello_Tile *
find_valid_move_for_tile(Othello_Board *board, Othello_Tile *tile, Othello_Tile tile_direction) {
    assert(tile_direction.kind == OthelloTileKind_FindDirection);
    
    Othello_Tile_Kind player = board->state->current_player;
    Othello_Tile_Kind last_tile_kind = OthelloTileKind_None;
    
    Othello_Tile *result = 0;
    
    s32 x = tile->x;
    s32 y = tile->y;
    while (1) {
        x += tile_direction.x;
        y += tile_direction.y;
        
        if (x < 0 || y < 0 || x >= OTHELLO_BOARD_COUNT || y >= OTHELLO_BOARD_COUNT) {
            break;
        }
        
        Othello_Tile *landed = &board->tiles[x][y];
        if (landed->kind == OthelloTileKind_None) {
            if (last_tile_kind != OthelloTileKind_None && last_tile_kind != player)
                result = landed;
            break;
        }
        
        last_tile_kind = landed->kind;
    }
    
    return result;
}

internal void
othello_menu_art(App_State *state, Vector2 min, Vector2 max) {
    immediate_begin();
    immediate_textured_quad(min, max, state->menu_art.othello);
    immediate_flush();
}

internal void
othello_game_restart(Othello_State *state) {
    init_game(state);
}

internal void
othello_game_update_and_render(Game_Memory *memory, Game_Input *input) {
    Othello_State *state = (Othello_State *) memory->permanent_storage;
    if (!memory->initialized) {
        assert(memory->permanent_storage_size == 0);
        assert(!memory->permanent_storage);
        memory->initialized = true;
        
        // This is used to store dynamic valid player moves.
        Memory_Index move_size = kilobytes(16);
        
        Memory_Index total_memory_size = move_size;
        Memory_Index total_available_size = total_memory_size - sizeof(Othello_State);
        
        state = (Othello_State *) game_alloc(memory, total_memory_size);
        
        Othello_Assets assets = {};
        assets.board_font = load_font("./data/fonts/Inconsolata-Regular.ttf", 32.f);
        assets.turn_font = load_font("./data/fonts/Inconsolata-Bold.ttf", 32.f);
        
        state->assets = assets;
        
        init_arena(&state->move_arena, total_available_size, (u8 *) memory->permanent_storage + sizeof(Othello_State));
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
                    othello_game_restart(state);
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
othello_game_free(Game_Memory *memory) {
    if (!memory) return;
    
    Othello_State *state = (Othello_State *) memory->permanent_storage;
    if (!state) return;
    
    // TODO(diego): If necessary.
}
