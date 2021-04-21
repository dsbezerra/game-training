internal void
clear_and_generate_board(Bejeweled_Board *board) {
    for (u32 x = 0; x < BEJEWELED_GRID_COUNT; ++x) {
        for (u32 y = 0; y < BEJEWELED_GRID_COUNT; ++y) {
            Bejeweled_Slot *slot = &board->slots[x][y];
            slot->type = BejeweledSlotType_Normal;
            slot->x = x;
            slot->y = y;
            random_gem_for_slot(board, slot);
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
            random_gem_for_slot(board, slot);}
    }
}

internal Bejeweled_Gem_List
possible_gems_for_slot(Bejeweled_Board *board, u32 slot_x, u32 slot_y) {
    Bejeweled_Gem_List result = {};
    
    Memory_Arena *arena = &board->state->board_arena;
    
    Bejeweled_Gem gems[] = {
        BejeweledGem_Purple,
        BejeweledGem_Green,
        BejeweledGem_Orange,
        BejeweledGem_Yellow,
        BejeweledGem_Pink,
        BejeweledGem_Black,
        BejeweledGem_White,
    };
    
    // Offset
    Bejeweled_Tile offsets[4] = {
        {1, 0}, {-1,  0}, // X
        {0, 1}, { 0, -1}, // Y
    };
    
    u32 num_possible_gems = array_count(gems);
    for (u32 offset_index = 0; offset_index < array_count(offsets); ++offset_index)
    {
        Bejeweled_Tile offset = offsets[offset_index];
        Bejeweled_Gem gem_at_offset = get_gem_at(board, slot_x + offset.x, slot_y + offset.y);
        if (gem_at_offset != BejeweledGem_None) {
            u32 index = ((u32) gem_at_offset) - 1;
            if (gems[index] != BejeweledGem_None) {
                gems[index] = BejeweledGem_None;
                num_possible_gems--;
            }
        }
    }
    result.gems = push_array(arena, num_possible_gems, Bejeweled_Gem);
    
    u32 index = 0;
    u32 added = 0;
    while (added < num_possible_gems && index < array_count(gems)) 
    {
        Bejeweled_Gem g = gems[index++];
        if (g == BejeweledGem_None)
            continue;
        
        result.gems[added] = g;
        added++;
    }
    
    result.num_gems = num_possible_gems;
    
    return result;
}

internal void
do_swap(Bejeweled_State *state) {
    Bejeweled_Board *board = &state->board;
    
    Bejeweled_Slot *slot_a = get_slot_at(board, state->swap.from.x, state->swap.from.y);
    Bejeweled_Slot *slot_b = get_slot_at(board, state->swap.to.x, state->swap.to.y);
    
    assert(slot_a && slot_b);
    swap_slots(slot_a, slot_b);
    clear_swap(&state->swap);
}

internal void
clear_swap(Bejeweled_Gem_Swap *swap) {
    swap->state = BejeweledSwapState_Idle;
    swap->from.x = -1;
    swap->from.y = -1;
    swap->to.x = -1;
    swap->to.y = -1;
}

internal void
swap_slots(Bejeweled_Slot *slot_a, Bejeweled_Slot *slot_b) {
    Bejeweled_Slot aux = *slot_a;
    
    slot_a->x = slot_b->x;
    slot_a->y = slot_b->y;
    slot_a->type = slot_b->type;
    slot_a->gem = slot_b->gem;
    
    slot_b->x = aux.x;
    slot_b->y = aux.y;
    slot_b->type = aux.type;
    slot_b->gem = aux.gem;
}

internal b32
is_swap_valid(Bejeweled_Gem_Swap swap) {
    b32 result = true;
    
    u32 x_difference = abs(swap.from.x - swap.to.x);
    u32 y_difference = abs(swap.from.y - swap.to.y);
    
    result = x_difference != y_difference && (x_difference == 1 || y_difference == 1);
    
    return result;
}

internal b32
is_tile_valid(Bejeweled_Tile tile) {
    b32 result = false;
    
    result = tile.x >= 0 && tile.x < BEJEWELED_GRID_COUNT && tile.y >= 0 && tile.y < BEJEWELED_GRID_COUNT;
    
    return result;
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

internal Bejeweled_Gem
get_gem_at(Bejeweled_Board *board, s32 x, s32 y) {
    Bejeweled_Gem result = BejeweledGem_None;
    
    if (x < 0 || x >= BEJEWELED_GRID_COUNT) return result;
    if (y < 0 || y >= BEJEWELED_GRID_COUNT) return result;
    
    Bejeweled_Slot *slot = &board->slots[x][y];
    result = slot->gem;
    
    return result;
}

internal Bejeweled_Slot *
get_slot_at(Bejeweled_Board *board, s32 x, s32 y) {
    if (x < 0 || x >= BEJEWELED_GRID_COUNT) return 0;
    if (y < 0 || y >= BEJEWELED_GRID_COUNT) return 0;
    
    Bejeweled_Slot *slot = &board->slots[x][y];
    return slot;
}

internal void
random_gem_for_slot(Bejeweled_Board *board, Bejeweled_Slot *slot) {
    Bejeweled_Gem_List possible_gems = possible_gems_for_slot(board, slot->x, slot->y);
    assert(possible_gems.num_gems > 0);
    
    u32 random_choice = random_int_in_range(0, possible_gems.num_gems-1);
    slot->gem = possible_gems.gems[random_choice];
}

internal Bejeweled_Tile
get_tile_under_mouse(Bejeweled_State *state) {
    Bejeweled_Tile result = {-1, -1};
    
    // @Hardcoded
    real32 tile_size = 64.f;
    
    Vector2 start = get_start_xy(state->memory->window_dimensions, tile_size);
    
    real32 max_x = start.x + tile_size * BEJEWELED_GRID_COUNT;
    real32 max_y = start.y + tile_size * BEJEWELED_GRID_COUNT;
    
    Vector2i mp = state->mouse_position;
    if (mp.x < start.x || mp.x > max_x) return result;
    if (mp.y < start.y || mp.y > max_y) return result;
    
    s32 x = (s32) ((mp.x - start.x) / tile_size);
    s32 y = (s32) ((mp.y - start.y) / tile_size);
    
    assert(x >= 0 && x < BEJEWELED_GRID_COUNT);
    assert(y >= 0 && y < BEJEWELED_GRID_COUNT);
    
    result.x = x;
    result.y = y;
    
    return result;
}

internal Vector2
get_start_xy(Vector2i dim, real32 tile_size) {
    real32 width = tile_size*BEJEWELED_GRID_COUNT;
    real32 height = tile_size*BEJEWELED_GRID_COUNT;
    
    real32 sx = center_horizontally((real32) dim.width, width);
    real32 sy = center_vertically((real32) dim.height, height);
    
    return make_vector2(sx, sy);
}

internal void
init_game(Bejeweled_State *state) {
    state->game_mode = GameMode_Playing;
    state->memory->game_mode = GameMode_Playing;
    state->board.state = state;
    state->swap = {};
    
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
    platform_get_cursor_position(&state->mouse_position);
    
    if (pressed(Button_Mouse1)) {
        handle_mouse_click(state);
    }
}

internal void
handle_mouse_click(Bejeweled_State *state) {
    Bejeweled_Tile tile = get_tile_under_mouse(state);
    if (is_tile_valid(tile)) {
        if (state->swap.state == BejeweledSwapState_Idle) {
            state->swap.from = tile;
            state->swap.state = BejeweledSwapState_From;
        } else if (state->swap.state == BejeweledSwapState_From) {
            state->swap.to = tile;
            state->swap.state = BejeweledSwapState_To;
            if (!is_swap_valid(state->swap))
                clear_swap(&state->swap);
            else
                do_swap(state);
        }
    }
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
    
    Vector2 start = get_start_xy(dim, tile_size);
    
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
                
                Vector2 min = make_vector2(start.x + tile_size * x, start.y + tile_size * y);
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
        
        Memory_Index board_memory_size = megabytes(8);
        Memory_Index total_memory_size = board_memory_size;
        Memory_Index total_available_size = total_memory_size - sizeof(Bejeweled_State);
        
        state = (Bejeweled_State *) game_alloc(memory, total_memory_size);
        state->memory = memory;
        
        
        Spritesheet *s = load_spritesheet("./data/textures/bejeweled/sprites.txt");
        state->main_sheet = s;
        init_spritesheet(s, UPLOAD_SPRITESHEET);
        init_arena(&state->board_arena, total_available_size, (u8 *) memory->permanent_storage + sizeof(Bejeweled_State));
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
