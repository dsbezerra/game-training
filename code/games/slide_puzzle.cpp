
internal void
generate_puzzle(slide_puzzle_state *state) {
    slide_puzzle_gen generation = {};
    generation.number = 1;
    generation.shuffle_t = .0f;
    generation.shuffle_target = 1.f;
    generation.fill_t = .0f;
    generation.fill_target = 1.f;
    generation.shuffle_index = SLIDE_PUZZLE_BOARD_COUNT*SLIDE_PUZZLE_BOARD_COUNT - 1;
    generation.tile_index = 1;
    state->mode = SlidePuzzleMode_Generating;
    state->generation = generation;
}

internal void 
init_puzzle(slide_puzzle_state *state) {
    // NOTE(diego): Default puzzle.
    
    s8 len = SLIDE_PUZZLE_BOARD_COUNT*SLIDE_PUZZLE_BOARD_COUNT;
    slide_puzzle_tile *tiles = (slide_puzzle_tile *) state->board.tiles;
    
    s8 number = 1;
    // NOTE(diego): First tile should be empty!!
    for (slide_puzzle_tile *tile = tiles + 1; tile != tiles + len; tile++) {
        tile->id = number;
        if (number < 10) {
            tile->content[0] = number + '0';
            tile->content[1] = '\0';
        } else if (number < len + 1) {
            int left = (int) (number / 10);
            int right = number % 10;
            tile->content[0] = (char) (left + '0');
            tile->content[1] = (char) (right + '0');
            tile->content[2] = '\0';
        }
        ++number;
    }
    
    //
    // Shuffle using Fisher–Yates algorithm 
    //
    // for i from n−1 downto 1 do
    //     j ← random integer such that 0 ≤ j ≤ i
    //     exchange a[j] and a[i]
    //
    for (int i = len - 1; i >= 1; --i) {
        int j = random_int_in_range(1, len - 1);
        slide_puzzle_tile *a = tiles + i;
        slide_puzzle_tile *b = tiles + j;
        slide_puzzle_tile aux = *a;
        *a = *b;
        *b = aux;
    }
}

internal void
init_puzzle(slide_puzzle_state *state, b32 animated) {
    if (animated) generate_puzzle(state);
    else init_puzzle(state);
}


internal void
slide_puzzle_game_restart(slide_puzzle_state *state) {
    state->game_mode = GameMode_Playing;
    
    //
    // Re-init
    //
    
    state->mode = SlidePuzzleMode_Generating;
    
    state->sliding_t_rate = 10.f;
    state->sliding_target = 1.f;
    
    state->swap.from_index = -1;
    state->swap.to_index = -1;
    
    generate_puzzle(state);
}


internal void
update_generating(slide_puzzle_state *state) {
    
    slide_puzzle_gen *generation = &state->generation;
    
    s8 len = SLIDE_PUZZLE_BOARD_COUNT*SLIDE_PUZZLE_BOARD_COUNT;
    
    real32 speed = 26.f;
    
    slide_puzzle_tile *tiles = (slide_puzzle_tile *) state->board.tiles;
    if (generation->fill_t >= generation->fill_target) {
        generation->fill_t -= generation->fill_target;
        
        slide_puzzle_tile *tile = tiles + generation->tile_index;
        tile->id = generation->number;
        if (generation->number < 10) {
            tile->content[0] = generation->number + '0';
            tile->content[1] = '\0';
        } else if (generation->number < len + 1) {
            int left = (int) (generation->number / 10);
            int right = generation->number % 10;
            tile->content[0] = (char) (left + '0');
            tile->content[1] = (char) (right + '0');
            tile->content[2] = '\0';
        }
        ++generation->tile_index;
        ++generation->number;
    } else if (generation->tile_index < len) {
        generation->fill_t += time_info.dt * speed;
    } else {
        
        if (generation->shuffle_t >= generation->shuffle_target) {
            generation->shuffle_t -= generation->shuffle_target;
            //
            // Shuffle using Fisher–Yates algorithm 
            //
            // for i from n−1 downto 1 do
            //     j ← random integer such that 0 ≤ j ≤ i
            //     exchange a[j] and a[i]
            //
            int j = random_int_in_range(1, len - 1);
            slide_puzzle_tile *a = tiles + generation->shuffle_index;
            slide_puzzle_tile *b = tiles + j;
            slide_puzzle_tile aux = *a;
            *a = *b;
            *b = aux;
            
            --generation->shuffle_index;
            
        } else if (generation->shuffle_index >= 1) {
            generation->shuffle_t += time_info.dt * speed;
        } else {
            state->mode = SlidePuzzleMode_Ready;
        }
    }
    
}

internal void
draw_board(slide_puzzle_state *state) {
    
    v2i dim = state->dimensions;
    
    real32 pad;
    if (dim.width > dim.height) {
        pad = dim.height * .001f;
    } else {
        pad = dim.width * .001f;
    }
    
    pad = clampf(1.f, pad, 2.f);
    
    real32 max_height = dim.height * .6f;
    
    
    real32 tile_size = (max_height - SLIDE_PUZZLE_BOARD_COUNT * pad) / (real32) SLIDE_PUZZLE_BOARD_COUNT;
    
    v2 start = make_v2((dim.width  - tile_size * SLIDE_PUZZLE_BOARD_COUNT) / 2.f,
                       (dim.height - tile_size * SLIDE_PUZZLE_BOARD_COUNT) / 2.f);
    
    real32 board_size = tile_size * SLIDE_PUZZLE_BOARD_COUNT;
    
    //
    // Draw tiles
    //
    immediate_begin();
    
    immediate_quad(make_v2(.0f, 0.f), make_v2((real32) dim.width, (real32) dim.height), make_color(0xff022c56), 1.f);
    
    b32 should_animate_slide = state->swap.to_index != -1 && state->swap.from_index != -1;
    int from_tile_x = state->swap.from_index % SLIDE_PUZZLE_BOARD_COUNT;
    int from_tile_y = state->swap.from_index / SLIDE_PUZZLE_BOARD_COUNT;
    int to_tile_x = state->swap.to_index % SLIDE_PUZZLE_BOARD_COUNT;
    int to_tile_y = state->swap.to_index / SLIDE_PUZZLE_BOARD_COUNT;
    
    v2 board_min = make_v2(start.x, start.y);
    v2 board_max = make_v2(board_min.x + board_size, board_min.y + board_size);
    
    immediate_quad(board_min, board_max, make_color(0xff008200), 1.f);
    
    for (s8 tile_y = 0; tile_y < SLIDE_PUZZLE_BOARD_COUNT; ++tile_y) {
        for (s8 tile_x = 0; tile_x < SLIDE_PUZZLE_BOARD_COUNT; ++tile_x) {
            slide_puzzle_tile tile = state->board.tiles[tile_x][tile_y];
            if (!tile.id) {
                continue;
            }
            
            v2 min = make_v2(tile_size * tile_x + pad,       
                             tile_size * tile_y + pad);
            v2 max = make_v2(tile_size * tile_x + tile_size, 
                             tile_size * tile_y + tile_size);
            
            // @Copy 'n' Paste
            if (should_animate_slide) {
                b32 from_this = tile_x == from_tile_x && tile_y == from_tile_y;
                b32 to_this   = tile_x == to_tile_x   && tile_y == to_tile_y;
                if (from_this || to_this) {
                    v2 from_min = make_v2(tile_size * from_tile_x + pad,       
                                          tile_size * from_tile_y + pad);
                    v2 from_max = make_v2(tile_size * from_tile_x + tile_size, 
                                          tile_size * from_tile_y + tile_size);
                    v2 to_min = make_v2(tile_size * to_tile_x + pad,       
                                        tile_size * to_tile_y + pad);
                    v2 to_max = make_v2(tile_size * to_tile_x + tile_size, 
                                        tile_size * to_tile_y + tile_size);
                    min = lerp_v2(to_min, state->sliding_t, from_min);
                    max = lerp_v2(to_max, state->sliding_t, from_max);
                }
            }
            
            // Translate values to correct position so it can be correctly centered.
            min = add_v2(min, start);
            max = add_v2(max, start);
            
            immediate_quad(min, max, make_color(0xff00cf00), 1.f);
        }
    }
    
    immediate_flush();
    
    //
    // Draw tile content
    //
    
    v4 white = make_color(0xffffffff);
    
    real32 half_tile_size = tile_size * .5f;
    loaded_font *tile_font = &state->assets.tile_font;
    
    immediate_begin();
    for (s8 tile_y = 0; tile_y < SLIDE_PUZZLE_BOARD_COUNT; ++tile_y) {
        for (s8 tile_x = 0; tile_x < SLIDE_PUZZLE_BOARD_COUNT; ++tile_x) {
            
            slide_puzzle_tile tile = state->board.tiles[tile_x][tile_y];
            if (!tile.id) {
                continue;
            }
            
            v2 min = make_v2(tile_size * tile_x + pad,       
                             tile_size * tile_y + pad);
            v2 max = make_v2(tile_size * tile_x + tile_size, 
                             tile_size * tile_y + tile_size);
            
            // @Copy 'n' Paste
            if (should_animate_slide) {
                b32 from_this = tile_x == from_tile_x && tile_y == from_tile_y;
                b32 to_this   = tile_x == to_tile_x   && tile_y == to_tile_y;
                if (from_this || to_this) {
                    v2 from_min = make_v2(tile_size * from_tile_x + pad,       
                                          tile_size * from_tile_y + pad);
                    v2 from_max = make_v2(tile_size * from_tile_x + tile_size, 
                                          tile_size * from_tile_y + tile_size);
                    v2 to_min = make_v2(tile_size * to_tile_x + pad,       
                                        tile_size * to_tile_y + pad);
                    v2 to_max = make_v2(tile_size * to_tile_x + tile_size, 
                                        tile_size * to_tile_y + tile_size);
                    min = lerp_v2(to_min, state->sliding_t, from_min);
                    max = lerp_v2(to_max, state->sliding_t, from_max);
                }
            }
            
            // Translate values to correct position so it can be correctly centered.
            min = add_v2(min, start);
            max = add_v2(max, start);
            
            real32 size = get_text_width(&state->assets.primary_font, (char *) tile.content);
            
            // TODO(diego): Diagnostic text width routine
            immediate_text(min.x + half_tile_size - size * .5f, min.y + half_tile_size + tile_font->line_height * .5f, (u8 *) tile.content, tile_font, white, 1.f);
        }
    }
    
    immediate_flush();
    
    
    if (state->mode == SlidePuzzleMode_Generating) {
        draw_text(dim.width * .05f, dim.height * 0.1f, (u8 *) "Generating puzzle...", &state->assets.generating_font, white);
    }
}

internal void
draw_game_view(slide_puzzle_state *state) {
    if (state->game_mode == GameMode_Playing) {
        draw_board(state);
    } else {
        draw_menu(SLIDE_PUZZLE_TITLE, state->dimensions, state->game_mode, state->menu_selected_item, state->quit_was_selected);
    }
}

internal void
slide_puzzle_game_update_and_render(game_memory *memory, game_input *input) {
    
    slide_puzzle_state *state = (slide_puzzle_state *) memory->permanent_storage;
    if (!memory->initialized) {
        assert(memory->permanent_storage_size == 0);
        assert(!memory->permanent_storage);
        memory->initialized = true;
        
        state = (slide_puzzle_state *) game_alloc(memory, megabytes(12));
        
        slide_puzzle_assets assets = {};
        assets.primary_font = load_font("./data/fonts/Inconsolata-Regular.ttf", 24.f);
        assets.generating_font = load_font("./data/fonts/Inconsolata-Bold.ttf", 42.f);
        assets.tile_font = load_font("./data/fonts/Inconsolata-Bold.ttf", 32.f);
        
        state->game_mode = GameMode_Playing;
        state->mode = SlidePuzzleMode_Generating;
        state->assets = assets;
        
        state->sliding_t_rate = 10.f;
        state->sliding_target = 1.f;
        
        state->swap.from_index = -1;
        state->swap.to_index = -1;
        
        generate_puzzle(state);
    }
    
    state->dimensions = memory->window_dimensions;
    
    //
    // Update
    //
    
    if (state->game_mode == GameMode_Playing) {
        if (state->mode == SlidePuzzleMode_Generating) {
            update_generating(state);
        } else if (state->mode == SlidePuzzleMode_Ready) {
            if (pressed(Button_Escape)) {
                state->game_mode = GameMode_Menu;
            } else {
                if (state->sliding_t == 0.f) {
                    s8 old_board_x = state->board.empty_index % SLIDE_PUZZLE_BOARD_COUNT;
                    s8 old_board_y = state->board.empty_index / SLIDE_PUZZLE_BOARD_COUNT;
                    
                    s8 new_board_x = old_board_x;
                    s8 new_board_y = old_board_y;
                    
                    if (pressed(Button_Up))
                        ++new_board_y;
                    if (pressed(Button_Down))
                        --new_board_y;
                    
                    if (pressed(Button_Right))
                        --new_board_x;
                    if (pressed(Button_Left))
                        ++new_board_x;
                    
                    b32 valid = new_board_x >= 0 && new_board_x < SLIDE_PUZZLE_BOARD_COUNT && new_board_y >= 0 && new_board_y < SLIDE_PUZZLE_BOARD_COUNT;
                    b32 changed = old_board_x != new_board_x || old_board_y != new_board_y;
                    // Check if new_empty_index is valid
                    if (valid && changed) {
                        slide_puzzle_tile old_tile = state->board.tiles[old_board_x][old_board_y];
                        slide_puzzle_tile new_tile = state->board.tiles[new_board_x][new_board_y];
                        
                        state->board.tiles[old_board_x][old_board_y] = new_tile;
                        state->board.tiles[new_board_x][new_board_y] = old_tile;
                        
                        s8 new_empty_index = new_board_x + new_board_y * SLIDE_PUZZLE_BOARD_COUNT;
                        
                        state->swap.from_index = state->board.empty_index;
                        state->swap.to_index = new_empty_index;
                        state->board.empty_index = new_empty_index;
                        state->sliding_t += time_info.dt;
                    }
                } else {
                    state->sliding_t += time_info.dt * state->sliding_t_rate;
                    if (state->sliding_t >= state->sliding_target) {
                        state->sliding_t = .0f;
                        state->swap.to_index = -1;
                        state->swap.from_index = -1;
                    }
                }
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
                    slide_puzzle_game_restart(state);
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