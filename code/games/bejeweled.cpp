internal void
clear_and_generate_board(Bejeweled_Board *board) {
    for (u32 x = 0; x < BEJEWELED_GRID_COUNT; ++x) {
        for (u32 y = 0; y < BEJEWELED_GRID_COUNT; ++y) {
            Bejeweled_Slot *slot = &board->slots[x][y];
            slot->type = BejeweledSlotType_Normal;
            slot->gem = get_random_gem(board->state);
            assert(slot->gem != BejeweledGem_None);
            
            slot->x = x;
            slot->y = y;
        }
    }
}

internal void
clear_board(Bejeweled_Board *board) {
    for (u32 x = 0; x < BEJEWELED_GRID_COUNT; ++x) {
        for (u32 y = 0; y < BEJEWELED_GRID_COUNT; ++y) {
            Bejeweled_Slot *slot = &board->slots[x][y];
            slot->type = BejeweledSlotType_Normal;
            slot->gem = BejeweledGem_None;
            slot->x = x;
            slot->y = y;
        }
    }
}

internal void
generate_board(Bejeweled_Board *board) {
    for (u32 x = 0; x < BEJEWELED_GRID_COUNT; ++x) {
        for (u32 y = 0; y < BEJEWELED_GRID_COUNT; ++y) {
            Bejeweled_Slot *slot = &board->slots[x][y];
            slot->gem = get_random_gem(board->state);
        }
    }
}

internal Bejeweled_Gem
get_random_gem(Bejeweled_State *state) {
    u32 random_slot = random_int_in_range(0, array_count(state->gems) - 1);
    return state->gems[random_slot].gem;
}

internal Sprite *
get_sprite(Bejeweled_State *state, Bejeweled_Gem gem) {
    Bejeweled_Gem_Sprite *g = &state->gems[(u32) gem - 1];
    return g->sprite;
}

internal void
init_game(Bejeweled_State *state) {
    state->game_mode = GameMode_Playing;
    state->memory->game_mode = GameMode_Playing;
    state->board.state = state;
    
#define BEJEWELED_GEM(GEM_TYPE) state->gems[GEM_TYPE-1].gem = GEM_TYPE
    BEJEWELED_GEM(BejeweledGem_Purple);
    BEJEWELED_GEM(BejeweledGem_Green);
    BEJEWELED_GEM(BejeweledGem_Orange);
    BEJEWELED_GEM(BejeweledGem_Yellow);
    BEJEWELED_GEM(BejeweledGem_Pink);
    BEJEWELED_GEM(BejeweledGem_Black);
    BEJEWELED_GEM(BejeweledGem_White);
    
    Spritesheet *sheet = state->main_sheet;
    for (u32 sprite_index = 0;
         sprite_index < sheet->num_sprites;
         ++sprite_index)
    {
        Sprite *spr = &sheet->sprites[sprite_index];
        if (spr->name) {
            if (spr->name[0] == 'g' && spr->name[1] == 'e' && spr->name[2] == 'm') {
                u32 gem_index = atoi(&spr->name[3]);
                Bejeweled_Gem_Sprite *gem_s = &state->gems[gem_index-1];
                gem_s->sprite = spr;
            }
        }
    }
    
    platform_show_cursor(true);
    
    clear_and_generate_board(&state->board);
}

internal void
update_game(Bejeweled_State *state, Game_Input *input) {
    // TODO(diego): Implement
}

internal void
draw_game_view(Bejeweled_State *state) {
    
    Vector2i dim = state->memory->window_dimensions;
    real32 size = (real32) dim.width;
    if (size > dim.height)
        size = (real32) dim.height;
    
    game_frame_begin(dim.width, dim.height);
    
    real32 padding = 1.f;
    
    real32 tile_size = 64.f;
    
    real32 width = tile_size*BEJEWELED_GRID_COUNT;
    real32 height = tile_size*BEJEWELED_GRID_COUNT;
    
    real32 sx = center_horizontally((real32) dim.width, width);
    real32 sy = center_vertically((real32) dim.height, height);
    
    Spritesheet *sheet = state->main_sheet;
    
    if (state->game_mode == GameMode_Playing) {
        //
        // Background
        //
        immediate_begin();
        immediate_quad(0.f, 0.f, (real32) dim.width, (real32) dim.height, make_color(0xff2f3242));
        immediate_flush();
        
        immediate_begin();
        for (u32 x = 0;
             x < BEJEWELED_GRID_COUNT;
             ++x) {
            for (u32 y = 0;
                 y < BEJEWELED_GRID_COUNT;
                 ++y) {
                
                Bejeweled_Slot *slot = &state->board.slots[x][y];
                Sprite *spr = get_sprite(state, slot->gem);
                
                real32 u0 = spr->x / (real32) sheet->width;
                real32 u1 = (spr->x + spr->width) / (real32) sheet->width;
                
                // Flip UVs
                real32 v0 = 1.f - (spr->y / (real32) sheet->height);
                real32 v1 = 1.f - ((spr->y + spr->height) / (real32) sheet->height);
                
                Vector2 top_left     = make_vector2(u0, v0);
                Vector2 top_right    = make_vector2(u1, v0);
                Vector2 bottom_left  = make_vector2(u0, v1);
                Vector2 bottom_right = make_vector2(u1, v1);
                
                Vector2 min = make_vector2(sx + tile_size * x, sy + tile_size * y);
                Vector2 max = make_vector2(min.x + tile_size, min.y + tile_size);
                immediate_textured_quad(min, max, sheet->texture_id, top_right, top_left, bottom_right, bottom_left);
            }
        }
        immediate_flush();
    } else {
        draw_menu(BEJEWELED_TITLE, state->memory);
    }
}

internal void
bejeweled_menu_art(App_State *state, Vector2 min, Vector2 max) {
    // TODO(diego): Implement
}

internal void
bejeweled_game_restart(Bejeweled_State *state) {
    init_game(state);
}

internal void
bejeweled_game_update_and_render(Game_Memory *memory, Game_Input *input) {
    Bejeweled_State *state = (Bejeweled_State *) memory->permanent_storage;
    if (!memory->initialized) {
        assert(memory->permanent_storage_size == 0);
        assert(!memory->permanent_storage);
        memory->initialized = true;
        
        Memory_Index total_memory_size = kilobytes(16);
        Memory_Index total_available_size = total_memory_size - sizeof(Bejeweled_State);
        
        state = (Bejeweled_State *) game_alloc(memory, total_memory_size);
        state->memory = memory;
        
        
        Spritesheet *s = load_spritesheet("./data/textures/bejeweled/sprites.txt");
        state->main_sheet = s;
        
        init_spritesheet(s, UPLOAD_SPRITESHEET);
        init_game(state);
    }
    
    Simulate_Game sim = game_simulate(memory, input, state->game_mode);
    switch (sim.operation) {
        case SimulateGameOp_Update: {
            update_game(state, input);
        } break;
        
        case SimulateGameOp_Restart: {
            bejeweled_game_restart(state);
        } break;
        
        default: break;
    }
    
    //
    // Draw
    //
    draw_game_view(state);
}

internal void
bejeweled_game_free(Game_Memory *memory) {
    if (!memory) return;
    
    Bejeweled_State *state = (Bejeweled_State *) memory->permanent_storage;
    if (!state) return;
    
    // TODO(diego): If necessary.
}
