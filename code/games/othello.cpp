internal void
init_game(Othello_State *state) {
    
    state->game_mode = GameMode_Playing;
    state->board.state = state;
    state->current_player = OthelloTileKind_Black;
    
    move_list_clear(state);
    
    reset_board(&state->board);
}

internal void
update_game(Othello_State *state, Game_Input *input) {
    
}

internal void
draw_board(Othello_State *state) {
    
    Vector2i dim = state->dimensions;
    
    // Board
    real32 start_x = .0f;
    real32 start_y = .0f;
    
    real32 height = dim.height * .9f;
    real32 tile_size = height / (real32) OTHELLO_BOARD_COUNT;
    
    real32 width  = tile_size * OTHELLO_BOARD_COUNT;
    
    {
        real32 remaining_wspace = dim.width - width;
        real32 remaining_hspace = dim.height - height;
        
        start_x = remaining_wspace * .5f;
        start_y = remaining_hspace * .5f;
        
        Vector2 min = make_vector2(start_x, start_y);
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
            Vector2 h_min = make_vector2(start_x, start_y + y_offset);
            Vector2 h_max = make_vector2(h_min.x + width, h_min.y + line_thickness);
            immediate_quad(h_min, h_max, line_color);
            
            // Vertical
            Vector2 v_min = make_vector2(start_x + x_offset, start_y);
            Vector2 v_max = make_vector2(v_min.x + line_thickness, v_min.y + height);
            immediate_quad(v_min, v_max, line_color);
            
            x_offset += tile_size;
            y_offset += tile_size;
        }
    }
    
    // Tiles
    {
        Vector4 white = make_color(0xffffffff);
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
                
                
                real32 sx = start_x + tile_x * tile_size;
                real32 sy = start_y + tile_y * tile_size;
                
                // Move to center of tile size
                Vector2 center = make_vector2(sx + tile_size * .5f, sy + tile_size * .5f);
                
                real32 radius = tile_size * 0.4f;
                immediate_circle_filled(center, radius, color);
            }
        }
    }
    
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
            
            real32 sx = start_x + move->x * tile_size;
            real32 sy = start_y + move->y * tile_size;
            
            // Move to center of tile size
            Vector2 center = make_vector2(sx + tile_size * .5f, sy + tile_size * .5f);
            
            real32 radius = tile_size * 0.4f;
            immediate_circle_filled(center, radius, color);
            
            move = move->next;
        }
    }
    
}

internal void
draw_game_view(Othello_State *state) {
    
    game_frame_begin(state->dimensions.width, state->dimensions.height);
    
    if (state->game_mode == GameMode_Playing) {
        immediate_begin();
        immediate_quad(0.f, 0.f, (real32) state->dimensions.width, (real32) state->dimensions.height, make_color(0xff2f3242));
        
        draw_board(state);
        
        immediate_flush();
    } else {
        draw_menu(OTHELLO_TITLE, state->dimensions, state->game_mode, state->menu_selected_item, state->quit_was_selected);
    }
}

internal void
reset_board(Othello_Board *board) {
    clear_board(board);
    
    set_tile(board, OthelloTileKind_White, 3, 3);
    set_tile(board, OthelloTileKind_Black, 4, 3);
    set_tile(board, OthelloTileKind_Black, 3, 4);
    set_tile(board, OthelloTileKind_White, 4, 4);
    
    move_list_update(board->state);
}

internal void
clear_board(Othello_Board *board) {
    for (u32 tile_x = 0; tile_x < OTHELLO_BOARD_COUNT; ++tile_x) {
        for (u32 tile_y = 0; tile_y < OTHELLO_BOARD_COUNT; ++tile_y) {
            Othello_Tile empty = {};
            empty.x = tile_x;
            empty.y = tile_y;
            board->tiles[tile_x][tile_y] = empty;
        }
    }
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
    Othello_Tile_Find_Direction direction = {};
    direction.x = x;
    direction.y = y;
    
    Othello_Tile *result = find_valid_move_for_tile(&state->board, tile, direction);
    if (result) {
        Othello_Move move = {};
        move.x = result->x;
        move.y = result->y;
        move_list_add_if_unique(state, move);
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

internal void
set_tile(Othello_Board *board, Othello_Tile_Kind kind, u32 tile_x, u32 tile_y) {
    // NOTE(diego): Assumes tile_x and tile_y are within bounds.
    Othello_Tile *tile = &board->tiles[tile_x][tile_y];
    
    tile->kind = kind;
    tile->x = tile_x;
    tile->y = tile_y;
}

internal Othello_Tile *
find_valid_move_for_tile(Othello_Board *board, Othello_Tile *tile, Othello_Tile_Find_Direction direction) {
    
    Othello_Tile_Kind player = board->state->current_player;
    Othello_Tile_Kind last_tile_kind = OthelloTileKind_None;
    
    Othello_Tile *result = 0;
    
    s32 x = tile->x;
    s32 y = tile->y;
    while (1) {
        x += direction.x;
        y += direction.y;
        
        if (x < 0 || y < 0 || x > OTHELLO_BOARD_COUNT || y > OTHELLO_BOARD_COUNT) {
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
    immediate_quad(min, max, make_color(0xffff00ff));
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
