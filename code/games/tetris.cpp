internal tetris_piece 
make_piece(tetromino kind, s8 offset = -4) {
    assert(kind != Tetromino_None);
    assert(kind < Tetromino_Count);
    
    tetris_piece result = {};
    
    result.rotation = TetrisPieceRotation_0;
    
    for (int block_index = 0; block_index < TETRIS_PIECE_BLOCK_COUNT; ++block_index) {
        result.blocks[block_index].x = TETRIS_GRID_X_COUNT / 2;
        result.blocks[block_index].y = offset;
        result.blocks[block_index].placed = true;
    }
    
    switch (kind) {
        case Tetromino_I: {
            result.color = make_color(0xFF0F9BD7);
            //result.blocks[0].y = offset;
            result.blocks[1].y = result.blocks[0].y + 1;
            result.blocks[2].y = result.blocks[1].y + 1;
            result.blocks[3].y = result.blocks[2].y + 1;
        } break;
        case Tetromino_O: {
            result.color = make_color(0xFFE39F02);
            // [0] Top left block
            // Top right block
            result.blocks[1].x = result.blocks[0].x + 1;
            // Bottom left block
            result.blocks[3].y = result.blocks[0].y + 1;
            // Bottom right block
            result.blocks[2].x = result.blocks[0].x + 1;
            result.blocks[2].y = result.blocks[0].y + 1;
            
        } break;
        case Tetromino_T: {
            result.color = make_color(0xFFAF298A);
            // [0] == Top middle block
            // Left bottom block
            result.blocks[1].x = result.blocks[0].x - 1;
            result.blocks[1].y = result.blocks[0].y + 1;
            // Middle bottom block
            result.blocks[2].y = result.blocks[0].y + 1;
            
            // Right bottom block
            result.blocks[3].x = result.blocks[0].x + 1;
            result.blocks[3].y = result.blocks[0].y + 1;
        } break;
        case Tetromino_S: {
            result.color = make_color(0xFF59B101);
            // Right blocks
            result.blocks[0].x += 2;
            result.blocks[1].x = result.blocks[0].x - 1;
            // Left blocks
            result.blocks[2].y = result.blocks[0].y + 1;
            result.blocks[3].x = result.blocks[1].x;
            result.blocks[3].y = result.blocks[2].y;
        } break;
        case Tetromino_Z: {
            result.color = make_color(0xFFD70F37);
            // Right blocks
            result.blocks[0].x -= 2;
            result.blocks[1].x = result.blocks[0].x + 1;
            // Left blocks
            result.blocks[2].y = result.blocks[0].y + 1;
            result.blocks[3].x = result.blocks[1].x;
            result.blocks[3].y = result.blocks[2].y;
        } break;
        case Tetromino_J: {
            result.color = make_color(0xFF2141C6);
            result.blocks[1].y = result.blocks[0].y + 1;
            result.blocks[2].y = result.blocks[1].y + 1;
            result.blocks[3].x = result.blocks[2].x - 1;
            result.blocks[3].y = result.blocks[2].y;
        } break;
        case Tetromino_L: {
            result.color = make_color(0xFFE35B02);
            result.blocks[1].y = result.blocks[0].y + 1;
            result.blocks[2].y = result.blocks[1].y + 1;
            result.blocks[3].x = result.blocks[2].x + 1;
            result.blocks[3].y = result.blocks[2].y;
        } break;
        default: {
            assert(!"Should not happen!");
        } break;
    }
    
    // Calculate min and max x, y
    s8 min_x = TETRIS_GRID_X_COUNT;
    s8 min_y = TETRIS_GRID_Y_COUNT;
    s8 max_x = 0;
    s8 max_y = 0;
    for (int block_index = 0; block_index < TETRIS_PIECE_BLOCK_COUNT; ++block_index) {
        tetris_block *block = &result.blocks[block_index];
        if (block->x < min_x) {
            min_x = block->x;
        }
        if (block->x > max_x) {
            max_x = block->x;
        }
        if (block->y < min_y) {
            min_y = block->y;
        }
        if (block->y > max_y) {
            max_y = block->y;
        }
        block->color = result.color;
    }
    
    result.kind = kind;
    
    return result;
}

internal tetris_piece
random_piece() {
    tetromino kind = Tetromino_None;
    while (kind == Tetromino_None) {
        if (random_choice(1)) {
            kind = Tetromino_I;
        } 
        if (random_choice(2)) {
            kind = Tetromino_O;
        } 
        if (random_choice(3)) {
            kind = Tetromino_T;
        }
        if (random_choice(4)) {
            kind = Tetromino_S;
        }
        if (random_choice(5)) {
            kind = Tetromino_Z;
        }
        if (random_choice(6)) {
            kind = Tetromino_J;
        }
        if (random_choice(7)) {
            kind = Tetromino_L;
        }
    }
    return make_piece(kind);
}

internal void
spawn_piece(tetris_state *state, b32 make_current) {
    if (make_current) {
        state->current_piece = random_piece();
    }
    
    state->next_piece = random_piece();
}

internal void
init_game(tetris_state *state) {
    state->game_mode = GameMode_Playing;
    state->move_t = .0f;
    state->move_t_target = 1.f;
    state->move_dt = 2.f;
    state->score = 0;
    state->level = 1;
    
    // Clear grid
    for (int y = 0; y < TETRIS_GRID_Y_COUNT; ++y) {
        for (int x = 0; x < TETRIS_GRID_X_COUNT; ++x) {
            state->grid[y][x].placed = false;
        }
    }
    
    spawn_piece(state, true);
}

internal b32
is_row_complete(tetris_state *state, s8 row) {
    b32 result = true;
    for (u8 col = 0; col < TETRIS_GRID_X_COUNT; ++col) {
        if (!state->grid[row][col].placed) {
            result = false;
            break;
        }
    }
    return result;
}

internal b32
is_inside_grid(s8 x, s8 y) {
    if (x < 0 || x >= TETRIS_GRID_X_COUNT)
        return false;
    if (y < 0 || y >= TETRIS_GRID_Y_COUNT)
        return false;
    
    return true;
}

internal b32
is_piece_position_valid(tetris_state *state, tetris_piece *piece) {
    b32 result = true;
    for (s8 block_index = 0; block_index < TETRIS_PIECE_BLOCK_COUNT; ++block_index) {
        tetris_block block = piece->blocks[block_index];
        if (!is_inside_grid(block.x, block.y) || state->grid[block.y][block.x].placed) {
            result = false;
            break;
        }
    }
    return result;
}

internal b32
is_valid_position(tetris_state *state, s8 x, s8 y, s8 adx, s8 ady) {
    x = x + adx;
    y = y + ady;
    if (adx != 0 && (x < 0 || x >= TETRIS_GRID_X_COUNT))
        return false;
    if (ady != 0 && (y < 0 || y >= TETRIS_GRID_Y_COUNT))
        return false;
    
    return !state->grid[y][x].placed;
}

internal b32
place_piece(tetris_state *state) {
    tetris_piece piece = state->current_piece;
    for (int block_index = 0; block_index < TETRIS_PIECE_BLOCK_COUNT; ++block_index) {
        tetris_block block = piece.blocks[block_index];
        if (!is_inside_grid(block.x, block.y)) {
            return true;
        }
        state->grid[block.y][block.x] = block;
    }
    state->current_piece = state->next_piece;
    
    // Check for complete rows and move all blocks one row below for each complete line
    s8 y = TETRIS_GRID_Y_COUNT - 1;
    u8 cleared = 0;
    while (y >= 0) {
        if (is_row_complete(state, y)) {
            for (int yy = y; yy > 0; --yy) {
                for (int x = 0; x < TETRIS_GRID_X_COUNT; ++x) {
                    state->grid[yy][x] = state->grid[yy - 1][x];
                }
            }
            for (int x = 0; x < TETRIS_GRID_X_COUNT; ++x) {
                state->grid[0][x].placed = false;
            }
            cleared++;
        } else {
            y--;
        }
    }
    
    state->score += cleared;
    
    spawn_piece(state, false);
    
    return false;
}

internal b32
is_move_allowed(tetris_state *state, s8 x, s8 y) {
    b32 result = true;
    
    for (s8 block_index = 0; block_index < TETRIS_PIECE_BLOCK_COUNT; ++block_index) {
        tetris_block block = state->current_piece.blocks[block_index];
        if (!is_valid_position(state, block.x, block.y, x, y)) {
            result = false;
            break;
        }
    }
    return result;
}

internal b32
current_will_land(tetris_state *state) {
    b32 result = false;
    
    for (s8 block_index = 0; block_index < TETRIS_PIECE_BLOCK_COUNT; ++block_index) {
        tetris_block block = state->current_piece.blocks[block_index];
        s8 new_y = block.y + 1;
        if (new_y >= 0) {
            if (new_y >= TETRIS_GRID_Y_COUNT - 1 || state->grid[new_y][block.x].placed) {
                result = true;
                break;
            }
        }
    }
    
    return result;
}

internal tetris_piece
rotate_I(tetris_piece *piece) {
    tetris_piece rotated = *piece;
    
    if (rotated.rotation == TetrisPieceRotation_0) {
        rotated.blocks[0].x = rotated.blocks[0].x;
        rotated.blocks[1].x = rotated.blocks[0].x + 1;
        rotated.blocks[2].x = rotated.blocks[0].x + 2;
        rotated.blocks[3].x = rotated.blocks[0].x + 3;
        
        rotated.blocks[0].y = rotated.blocks[1].y;
        rotated.blocks[1].y = rotated.blocks[1].y;
        rotated.blocks[2].y = rotated.blocks[1].y;
        rotated.blocks[3].y = rotated.blocks[1].y;
        
        rotated.rotation = TetrisPieceRotation_1;
    } else if (rotated.rotation == TetrisPieceRotation_1) {
        rotated.blocks[0].x = rotated.blocks[1].x;
        rotated.blocks[1].x = rotated.blocks[1].x;
        rotated.blocks[2].x = rotated.blocks[1].x;
        rotated.blocks[3].x = rotated.blocks[1].x;
        
        rotated.blocks[0].y = rotated.blocks[0].y;
        rotated.blocks[1].y = rotated.blocks[0].y + 1;
        rotated.blocks[2].y = rotated.blocks[0].y + 2;
        rotated.blocks[3].y = rotated.blocks[0].y + 3;
        
        rotated.rotation = TetrisPieceRotation_0;
    }
    
    return rotated;
}

internal tetris_piece
rotate_T(tetris_piece *piece) {
    tetris_piece rotated = *piece;
    if (rotated.rotation == TetrisPieceRotation_0) {
        rotated.blocks[1].x = rotated.blocks[2].x;
        rotated.blocks[1].y += 1;
        rotated.rotation = TetrisPieceRotation_1;
    } else if (rotated.rotation == TetrisPieceRotation_1) {
        rotated.blocks[0].x -= 1;
        rotated.blocks[0].y = rotated.blocks[2].y;
        rotated.rotation = TetrisPieceRotation_2;
    } else if (rotated.rotation == TetrisPieceRotation_2) {
        rotated.blocks[1].y -= 2;
        rotated.blocks[3].x -= 1;
        rotated.blocks[3].y += 1;
        rotated.rotation = TetrisPieceRotation_3;
    } else if (rotated.rotation == TetrisPieceRotation_3) {
        rotated.blocks[0].x += 1;
        rotated.blocks[0].y -= 1;
        rotated.blocks[1].x -= 1;
        rotated.blocks[1].y += 1;
        rotated.blocks[3].x += 1;
        rotated.blocks[3].y -= 1;
        rotated.rotation = TetrisPieceRotation_0;
    }
    return rotated;
}

internal tetris_piece
rotate_S(tetris_piece *piece) {
    tetris_piece rotated = *piece;
    if (rotated.rotation == TetrisPieceRotation_0) {
        rotated.blocks[0].y += 1;
        rotated.blocks[1].x += 1;
        rotated.blocks[2].x += 1; rotated.blocks[2].y -= 2;
        rotated.blocks[3].y -= 1;
        rotated.rotation = TetrisPieceRotation_1;
    } else if (rotated.rotation == TetrisPieceRotation_1) {
        rotated.blocks[0].y -= 2;
        rotated.blocks[1].x -= 1; rotated.blocks[1].y -= 1;
        rotated.blocks[2].x -= 1; rotated.blocks[2].y += 1;
        rotated.rotation = TetrisPieceRotation_0;
    }
    return rotated;
}

internal tetris_piece
rotate_Z(tetris_piece *piece) {
    tetris_piece rotated = *piece;
    if (rotated.rotation == TetrisPieceRotation_0) {
        rotated.blocks[0].y += 1;
        rotated.blocks[1].x -= 1;
        rotated.blocks[2].x -= 1; rotated.blocks[2].y -= 2;
        rotated.blocks[3].y -= 1;
        rotated.rotation = TetrisPieceRotation_1;
    } else if (rotated.rotation == TetrisPieceRotation_1) {
        rotated.blocks[0].y -= 1;
        rotated.blocks[1].x += 1;
        rotated.blocks[2].x += 1; rotated.blocks[2].y += 2;
        rotated.blocks[3].y += 1;
        rotated.rotation = TetrisPieceRotation_0;
    }
    return rotated;
}

internal tetris_piece
rotate_L(tetris_piece *piece) {
    tetris_piece rotated = *piece;
    if (rotated.rotation == TetrisPieceRotation_0) {
        rotated.blocks[0].x += 2; rotated.blocks[0].y += 1;
        rotated.blocks[1].x += 1;
        rotated.blocks[2].y -= 1;
        rotated.blocks[3].x -= 1;
        rotated.rotation = TetrisPieceRotation_1;
    } else if (rotated.rotation == TetrisPieceRotation_1) {
        rotated.blocks[0].x -= 1;
        rotated.blocks[1].y -= 1;
        rotated.blocks[2].x += 1; rotated.blocks[2].y -= 2;
        rotated.blocks[3].y -= 3;
        rotated.rotation = TetrisPieceRotation_2;
    } else if (rotated.rotation == TetrisPieceRotation_2) {
        rotated.blocks[0].x -= 3; rotated.blocks[0].y -= 1;
        rotated.blocks[1].x -= 2;
        rotated.blocks[2].x -= 1; rotated.blocks[2].y += 1;
        rotated.rotation = TetrisPieceRotation_3;
    } else if (rotated.rotation == TetrisPieceRotation_3) {
        rotated.blocks[0].x += 2; rotated.blocks[0].y -= 1;
        rotated.blocks[1].x += 1;
        rotated.blocks[2].y += 1;
        rotated.blocks[3].x += 1; rotated.blocks[3].y += 2;
        rotated.rotation = TetrisPieceRotation_0;
    }
    return rotated;
}

internal tetris_piece
rotate_J(tetris_piece *piece) {
    tetris_piece rotated = *piece;
    if (rotated.rotation == TetrisPieceRotation_0) {
        rotated.blocks[0].x += 1; rotated.blocks[0].y += 2;
        rotated.blocks[1].y += 1;
        rotated.blocks[2].x -= 1;
        rotated.blocks[3].y -= 1;
        rotated.rotation = TetrisPieceRotation_1;
    } else if (rotated.rotation == TetrisPieceRotation_1) {
        rotated.blocks[0].x -= 2;
        rotated.blocks[1].x -= 1; rotated.blocks[1].y -= 1;
        rotated.blocks[2].y -= 2;
        rotated.blocks[3].x += 1; rotated.blocks[3].y -= 1;
        rotated.rotation = TetrisPieceRotation_2;
    } else if (rotated.rotation == TetrisPieceRotation_2) {
        rotated.blocks[0].x -= 1; rotated.blocks[0].y -= 2;
        rotated.blocks[1].y -= 1;
        rotated.blocks[2].x += 1;
        rotated.blocks[3].y += 1;
        rotated.rotation = TetrisPieceRotation_3;
    } else if (rotated.rotation == TetrisPieceRotation_3) {
        rotated.blocks[0].x += 2; rotated.blocks[0].y -= 1;
        rotated.blocks[1].x += 1;
        rotated.blocks[2].y += 1;
        rotated.blocks[3].x -= 1;
        rotated.rotation = TetrisPieceRotation_0;
    }
    
    return rotated;
}

internal void
rotate_piece(tetris_state *state) {
    tetris_piece *piece = &state->current_piece;
    tetris_piece rotated = *piece;
    
    switch (piece->kind) {
        case Tetromino_I: rotated = rotate_I(piece); break;
        case Tetromino_T: rotated = rotate_T(piece); break;
        case Tetromino_S: rotated = rotate_S(piece); break;
        case Tetromino_Z: rotated = rotate_Z(piece); break;
        case Tetromino_L: rotated = rotate_L(piece); break;
        case Tetromino_J: rotated = rotate_J(piece); break;
        default: { /* No-op */ } break;
    }
    
    if (is_piece_position_valid(state, &rotated)) {
        *piece = rotated;
    }
}

internal void
move_piece(tetris_state *state, tetris_move_direction direction = TetrisMoveDirection_Down) {
    assert(direction >= TetrisMoveDirection_Down && direction <= TetrisMoveDirection_Left);
    
    tetris_piece *piece = &state->current_piece;
    
    if (direction == TetrisMoveDirection_Left  && !is_move_allowed(state, -1, 0))
        return;
    if (direction == TetrisMoveDirection_Right && !is_move_allowed(state,  1, 0))
        return;
    
    if (direction == TetrisMoveDirection_Down && current_will_land(state)) {
        b32 game_over = place_piece(state);
        if (game_over) {
            state->game_mode = GameMode_GameOver;
        }
        return;
    }
    
    for (int block_index = 0; block_index < TETRIS_PIECE_BLOCK_COUNT; ++block_index) {
        tetris_block *block = &piece->blocks[block_index];
        if (direction == TetrisMoveDirection_Right) {
            block->x++;
        } else if (direction == TetrisMoveDirection_Left) {
            block->x--; 
        } else {
            block->y++;
        }
    }
}

internal void
draw_grid(tetris_state *state) {
    
    v2i dim = state->dimensions;
    
    real32 width  = dim.width  * .7f;
    real32 height = dim.height * .7f;
    
    real32 block_size = width / (real32) TETRIS_GRID_X_COUNT;
    if (width > dim.height) {
        block_size = (real32) height / (real32) TETRIS_GRID_Y_COUNT;
    }
    
    real32 grid_w = block_size * TETRIS_GRID_X_COUNT;
    real32 grid_h = block_size * (TETRIS_GRID_Y_COUNT - 1);
    
    real32 start_x = .0f + dim.width  * .5f - grid_w * .5f;
    real32 start_y = .0f + dim.height * .3f;
    
    for (int y = 0; y < TETRIS_GRID_Y_COUNT; ++y) {
        for (int x = 0; x < TETRIS_GRID_X_COUNT; ++x) {
            tetris_block block = state->grid[y][x];
            if (!block.placed) {
                continue;
            }
            v2 min = make_v2(start_x + x * block_size, start_y + y * block_size);
            v2 max = make_v2(min.x + block_size, min.y + block_size);
            immediate_quad(min, max, block.color);
        }
    }
    
    real32 border_size = 2.f;
    v4 border_color = make_color(0xff0000ff);
    {
        // Top border
        v2 min = make_v2(start_x, start_y);
        v2 max = make_v2(min.x + grid_w, min.y + border_size);
        immediate_quad(min, max, border_color); 
    }
    {
        // Bottom border
        v2 min = make_v2(start_x, start_y + grid_h - border_size);
        v2 max = make_v2(min.x + grid_w, min.y + border_size);
        immediate_quad(min, max, border_color); 
    }
    {
        // Left border
        v2 min = make_v2(start_x, start_y);
        v2 max = make_v2(min.x + border_size, min.y + grid_h);
        immediate_quad(min, max, border_color); 
    }
    {
        // Right border
        v2 min = make_v2(start_x + grid_w - border_size, start_y);
        v2 max = make_v2(min.x + border_size, min.y + grid_h);
        immediate_quad(min, max, border_color); 
    }
}

internal void
draw_current_piece(tetris_state *state) {
    
    v2i dim = state->dimensions;
    
    real32 width  = dim.width  * .7f;
    real32 height = dim.height * .7f;
    
    real32 block_size = width / (real32) TETRIS_GRID_X_COUNT;
    if (width > dim.height) {
        block_size = (real32) height / (real32) TETRIS_GRID_Y_COUNT;
    }
    
    v2 start = make_v2((dim.width - block_size * TETRIS_GRID_X_COUNT) / 2.f, dim.height - height);
    
    for (int block_index = 0; block_index < TETRIS_PIECE_BLOCK_COUNT; ++block_index) {
        tetris_block block = state->current_piece.blocks[block_index];
        v2 min = make_v2(block.x * block_size, block.y * block_size);
        v2 max = make_v2(min.x + block_size, min.y + block_size);
        min = add(min, start);
        max = add(max, start);
        immediate_quad(min, max, state->current_piece.color);
    }
}

internal void
draw_next_piece(tetris_state *state) {
    immediate_begin();
    
    real32 width  = state->dimensions.width  * .7f;
    real32 height = state->dimensions.height * .7f;
    
    real32 block_size = width / (real32) TETRIS_GRID_X_COUNT;
    if (width > state->dimensions.height) {
        block_size = (real32) height / (real32) TETRIS_GRID_Y_COUNT;
    }
    
    real32 start_x = width  * .9f;
    real32 start_y = height * .62f;
    
    for (int block_index = 0; block_index < TETRIS_PIECE_BLOCK_COUNT; ++block_index) {
        tetris_block block = state->next_piece.blocks[block_index];
        v2 min = make_v2(start_x + block.x * block_size, start_y + block.y * block_size);
        v2 max = make_v2(min.x + block_size, min.y + block_size);
        immediate_quad(min, max, state->next_piece.color);
    }
    
    immediate_flush();
}

internal void
draw_hud(tetris_state *state) {
    draw_next_piece(state);
    
    real32 width  = state->dimensions.width  * .7f;
    real32 height = state->dimensions.height * .7f;
    
    real32 start_x = width  * .9f;
    real32 y_cursor = height * .2f;
    
    v4 white = make_color(0xffffffff);
    
    char score[256];
    wsprintf(score, "Score: %d", state->score);
    draw_text(start_x, y_cursor, (u8 *) score, &state->assets.hud_font, white);
    
    y_cursor += state->assets.hud_font.line_height * 2.f;
    
    char level[256];
    wsprintf(level, "Level: %d", state->level);
    draw_text(start_x, y_cursor, (u8 *) level, &state->assets.hud_font, white);
    
    y_cursor = height * .5f;
    draw_text(start_x, y_cursor, (u8 *) "Next:", &state->assets.hud_font, white);
}

internal void
draw_game_view(tetris_state *state) {
    if (state->game_mode == GameMode_Playing) {
        
        immediate_begin();
        draw_current_piece(state);
        draw_grid(state);
        immediate_flush();
        
        draw_hud(state);
    } else {
        draw_menu(TETRIS_TITLE, state->dimensions, state->game_mode, state->menu_selected_item, state->quit_was_selected);
    }
}

internal void
tetris_menu_art(app_state *state, v2 min, v2 max) {
    immediate_begin();
    immediate_textured_quad(min, max, state->menu_art.tetris);
    immediate_flush();
}

internal void
tetris_game_restart(tetris_state *state) {
    init_game(state);
}

internal void
tetris_game_update_and_render(game_memory *memory, game_input *input) {
    
    tetris_state *state = (tetris_state *) memory->permanent_storage;
    if (!memory->initialized) {
        assert(memory->permanent_storage_size == 0);
        assert(!memory->permanent_storage);
        memory->initialized = true;
        
        state = (tetris_state *) game_alloc(memory, megabytes(12));
        state->assets.primary_font = load_font("./data/fonts/Inconsolata-Regular.ttf", 12.f);
        state->assets.hud_font = load_font("./data/fonts/Inconsolata-Regular.ttf", 24.f);
        
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
            if (pressed(Button_Left)) {
                move_piece(state, TetrisMoveDirection_Left);
            }
            if (pressed(Button_Right)) {
                move_piece(state, TetrisMoveDirection_Right);
            }
            if (is_down(Button_Down)) {
                state->move_dt *= 2.f;
                state->move_dt = clampf(2.f, state->move_dt, 64.f);
            } else {
                state->move_dt = 2.f;
            }
            
            if (pressed(Button_Up) || pressed(Button_Space)) {
                rotate_piece(state);
            }
            
            if (state->move_t >= state->move_t_target) {
                state->move_t -= state->move_t_target;
                move_piece(state);
            } else {
                state->move_t += time_info.dt * state->move_dt;
            }
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
