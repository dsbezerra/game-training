internal void
init_puzzle(slide_puzzle_state *state) {
    // NOTE(diego): Default puzzle.
    
    s8 len = SLIDE_PUZZLE_BOARD_COUNT*SLIDE_PUZZLE_BOARD_COUNT;
    slide_puzzle_tile *tiles = (slide_puzzle_tile *) state->board.tiles;
    
    s8 number = 1;
    // NOTE(diego): First tile should be empty!!
    for (slide_puzzle_tile *tile = tiles + 1; tile != tiles + len; tile++) {
        tile->id = number++;
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
    }
    
    //
    // TODO(diego):
    //
    // Shuffle using Fisher–Yates algorithm 
    //
    // for i from n−1 downto 1 do
    //     j ← random integer such that 0 ≤ j ≤ i
    //     exchange a[j] and a[i]
    //
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
    
    //
    // Draw tiles
    //
    immediate_begin();
    
    immediate_quad(make_v2(.0f, 0.f), make_v2((real32) dim.width, (real32) dim.height), make_color(0xff022c56), 1.f);
    
    for (s8 tile_y = 0; tile_y < SLIDE_PUZZLE_BOARD_COUNT; ++tile_y) {
        for (s8 tile_x = 0; tile_x < SLIDE_PUZZLE_BOARD_COUNT; ++tile_x) {
            v2 min = make_v2(tile_size * tile_x + pad,       
                             tile_size * tile_y + pad);
            v2 max = make_v2(tile_size * tile_x + tile_size, 
                             tile_size * tile_y + tile_size);
            // Translate values to correct position so it can be correctly centered.
            min = add_v2(min, start);
            max = add_v2(max, start);
            
            slide_puzzle_tile tile = state->board.tiles[tile_x][tile_y];
            if (!tile.id) {
                immediate_quad(min, max, make_color(0xff008200), 1.f);
            } else {
                immediate_quad(min, max, make_color(0xff00cf00), 1.f);
            }
            
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
            // Translate values to correct position so it can be correctly centered.
            min = add_v2(min, start);
            max = add_v2(max, start);
            
            real32 size = get_text_width(&state->assets.primary_font, (char *) tile.content);
            
            // TODO(diego): Diagnostic text width routine
            immediate_text(min.x + half_tile_size - size * .5f, min.y + half_tile_size + tile_font->line_height * .5f, (u8 *) tile.content, tile_font, white, 1.f);
        }
    }
    
    immediate_flush();
}

internal void
draw_game_view(slide_puzzle_state *state) {
    if (state->game_mode == GameMode_Playing) {
        draw_board(state);
    } else {
        //draw_menu(SLIDE_PUZZLE_TITLE, state->world.dimensions, state->game_mode, state->menu_selected_item, state->quit_was_selected);
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
        assets.tile_font = load_font("./data/fonts/Inconsolata-Bold.ttf", 32.f);
        
        
        state->game_mode = GameMode_Playing;
        state->assets = assets;
        
        init_puzzle(state);
    }
    
    state->dimensions = memory->window_dimensions;
    
    draw_game_view(state);
}