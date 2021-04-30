internal void
clear_and_generate_board(Bejeweled_Board *board) {
    Bejeweled_State *state = board->state;
    
    Vector2i dim = state->memory->window_dimensions;
    
    real32 tile_size = 64.f;
    real32 half_tile_size = tile_size * .5f;
    Vector2 start = get_start_xy(dim, tile_size);
    
    for (u32 x = 0; x < BEJEWELED_GRID_COUNT; ++x) {
        for (u32 y = 0; y < BEJEWELED_GRID_COUNT; ++y) {
            Bejeweled_Slot *slot = &board->slots[x][y];
            slot->type = BejeweledSlotType_Normal;
            slot->x = x;
            slot->y = y;
            slot->tile_size = tile_size;
            slot->half_tile_size = half_tile_size;
            slot->center = make_vector2(start.x + tile_size * x + half_tile_size, start.y + tile_size * y + half_tile_size);
            random_gem_for_slot(board, slot);
        }
    }
}

internal void
clear_board(Bejeweled_Board *board) {
    Bejeweled_State *state = board->state;
    
    Vector2i dim = state->memory->window_dimensions;
    
    real32 tile_size = 64.f;
    real32 half_tile_size = tile_size * .5f;
    Vector2 start = get_start_xy(dim, tile_size);
    
    for (u32 x = 0; x < BEJEWELED_GRID_COUNT; ++x) {
        for (u32 y = 0; y < BEJEWELED_GRID_COUNT; ++y) {
            Bejeweled_Slot *slot = &board->slots[x][y];
            slot->type = BejeweledSlotType_Normal;
            slot->gem = BejeweledGem_None;
            slot->x = x;
            slot->y = y;
            slot->tile_size = tile_size;
            slot->half_tile_size = half_tile_size;
            slot->center = make_vector2(start.x + tile_size * x + half_tile_size, start.y + tile_size * y + half_tile_size);
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
    
    Temporary_Memory gem_memory = begin_temporary_memory(arena);
    
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
    
    end_temporary_memory(gem_memory);
    
    return result;
}

internal void
prepare_swap(Bejeweled_State *state, Bejeweled_Gem_Swap *swap) {
    Vector2i dim = state->memory->window_dimensions;
    
    real32 tile_size = 64.f;
    real32 half_tile_size = tile_size * .5f;
    Vector2 start = get_start_xy(dim, tile_size);
    
    swap->from_center = make_vector2(start.x + tile_size * swap->from.x + half_tile_size, start.y + tile_size * swap->from.y + half_tile_size);
    swap->to_center = make_vector2(start.x + tile_size * swap->to.x + half_tile_size, start.y + tile_size * swap->to.y + half_tile_size);
    swap->state = BejeweledSwapState_Prepared;
    
    state->swap = *swap;
}

internal void
start_swap(Bejeweled_State *state, Bejeweled_Gem_Swap *swap) {
    swap->state = BejeweledSwapState_Swapping;
    swap->t = .0f;
    swap->duration = 0.3f;
}

internal void
do_swap(Bejeweled_State *state, Bejeweled_Gem_Swap *swap) {
    Bejeweled_Board *board = &state->board;
    
    Bejeweled_Slot *slot_a = get_slot_at(board, swap->from.x, swap->from.y);
    Bejeweled_Slot *slot_b = get_slot_at(board, swap->to.x, swap->to.y);
    
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
    swap->state = BejeweledSwapState_Idle;
    swap->t = .0f;
}

internal void
swap_slots(Bejeweled_Slot *slot_a, Bejeweled_Slot *slot_b) {
    Bejeweled_Slot aux = *slot_a;
    copy_slot(slot_a, *slot_b);
    copy_slot(slot_b, aux);
}

internal void
copy_slot(Bejeweled_Slot *slot_dest, Bejeweled_Slot slot_source) {
    slot_dest->type = slot_source.type;
    slot_dest->gem = slot_source.gem;
    slot_dest->tile_size = slot_source.tile_size;
    slot_dest->half_tile_size = slot_source.half_tile_size;
    slot_dest->center = slot_source.center;
    slot_dest->uv00 = slot_source.uv00;
    slot_dest->uv10 = slot_source.uv10;
    slot_dest->uv01 = slot_source.uv01;
    slot_dest->uv11 = slot_source.uv11;
}

internal b32
is_swap_valid(Bejeweled_Gem_Swap swap) {
    
    u32 x_difference = abs(swap.to.x - swap.from.x);
    if (x_difference > 1) return false;
    
    u32 y_difference = abs(swap.to.y - swap.from.y);
    if (y_difference > 1) return false;
    
    if (x_difference == y_difference) return false;
    
    return true;
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
    assert((s32)slot->x == x && (s32)slot->y == y);
    
    return slot;
}

internal void
random_gem_for_slot(Bejeweled_Board *board, Bejeweled_Slot *slot) {
    Bejeweled_Gem_List possible_gems = possible_gems_for_slot(board, slot->x, slot->y);
    assert(possible_gems.num_gems > 0);
    
    u32 random_choice = random_int_in_range(0, possible_gems.num_gems-1);
    slot->gem = possible_gems.gems[random_choice];
    
    // Fill UVs for Gem
    Bejeweled_State *state = board->state;
    
    Sprite *spr = get_sprite(state, slot->gem);
    assert(spr);
    
    Spritesheet *sheet = state->main_sheet;
    
    real32 u0 = spr->x / (real32) sheet->width;
    real32 u1 = (spr->x + spr->width) / (real32) sheet->width;
    
    // Flip UVs
    real32 v0 = 1.f - (spr->y / (real32) sheet->height);
    real32 v1 = 1.f - ((spr->y + spr->height) / (real32) sheet->height);
    
    slot->uv00 = make_vector2(u0, v0);
    slot->uv10 = make_vector2(u1, v0);
    slot->uv01 = make_vector2(u0, v1);
    slot->uv11 = make_vector2(u1, v1);
}

internal Bejeweled_Tile
get_tile_under_xy(Bejeweled_State *state, s32 x, s32 y) {
    Bejeweled_Tile result = {-1, -1};
    
    // @Hardcoded
    real32 tile_size = 64.f;
    
    Vector2 start = get_start_xy(state->memory->window_dimensions, tile_size);
    
    real32 max_x = start.x + tile_size * BEJEWELED_GRID_COUNT;
    real32 max_y = start.y + tile_size * BEJEWELED_GRID_COUNT;
    
    if (x < start.x || x > max_x) return result;
    if (y < start.y || y > max_y) return result;
    
    s32 tile_x = (s32) ((x - start.x) / tile_size);
    s32 tile_y = (s32) ((y - start.y) / tile_size);
    
    assert(tile_x >= 0 && tile_x < BEJEWELED_GRID_COUNT);
    assert(tile_y >= 0 && tile_y < BEJEWELED_GRID_COUNT);
    
    result.x = tile_x;
    result.y = tile_y;
    
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
        Sprite *spr = get_sprite(sheet, sprite_index);
        if (spr && spr->name) {
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
    handle_mouse(state, input);
    handle_swap(state);
}

internal void
handle_swap(Bejeweled_State *state) {
    
    Bejeweled_Gem_Swap *s = &state->swap;
    
    if (s->state == BejeweledSwapState_Prepared) {
        start_swap(state, s);
    } else if (s->state == BejeweledSwapState_Swapping) {
        
        real32 t = clampf(0.f, s->t / s->duration, 1.f);
        
        Bejeweled_Slot *slot_a = get_slot_at(&state->board, s->from.x, s->from.y);
        Bejeweled_Slot *slot_b = get_slot_at(&state->board, s->to.x, s->to.y);
        slot_a->center = lerp_vector2(s->from_center, t, s->to_center);
        slot_b->center = lerp_vector2(s->to_center  , t, s->from_center);
        if (t >= 1.f) {
            do_swap(state, s);
            return;
        }
        s->t += core.time_info.dt;
    }
}

internal void
handle_mouse(Bejeweled_State *state, Game_Input *input) {
    platform_get_cursor_position(&state->mouse_position);
    
    if (state->swap.state == BejeweledSwapState_Swapping || state->swap.state == BejeweledSwapState_Prepared) return;
    
    if (pressed(Button_Mouse1)) {
        state->pressed_t = core.time_info.current_time;
        state->pressed_position = state->mouse_position;
    }
    
    if (released(Button_Mouse1)) {
        
        Vector2i press_pos = state->pressed_position;
        Vector2i rel_pos = state->mouse_position;
        
        s32 dx = rel_pos.x - press_pos.x;
        s32 dy = rel_pos.y - press_pos.y;
        
        real32 now = core.time_info.current_time;
        real32 threshold = (real32) sqrt(dx*dx + dy*dy);
#define SWIPE_SLOP 10.f
        if (now - state->pressed_t > 1.f || threshold < SWIPE_SLOP) {
            return;
        }
        state->pressed_t = .0f;
        
        Bejeweled_Tile tile = get_tile_under_xy(state, press_pos.x, press_pos.y);
        if (!is_tile_valid(tile)) return;
        
        Bejeweled_Gem_Swap swap = {};
        swap.from = tile;
        swap.state = BejeweledSwapState_From;
        
        s32 adx = abs(dx);
        s32 ady = abs(dy);
        
        if (adx > ady) {
            // Process right/left swipe
            s32 offset = dx > 0 ? 1 : -1;
            swap.to = Bejeweled_Tile{tile.x+offset,tile.y};
            swap.state = BejeweledSwapState_To;
        } else if (adx < ady) {
            s32 offset = dy > 0 ? 1 : -1;
            swap.to = Bejeweled_Tile{tile.x,tile.y+offset};
            swap.state = BejeweledSwapState_To;
        } else {
            clear_swap(&swap);
        }
        
        if (is_swap_valid(swap)) {
            prepare_swap(state, &swap);
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
    
    if (state->game_mode == GameMode_Playing) {
        
        //
        // Background
        //
        immediate_begin();
        immediate_quad(0.f, 0.f, (real32) dim.width, (real32) dim.height, make_color(0xff2f3242));
        immediate_flush();
        
        Spritesheet *sheet = state->main_sheet;
        
        immediate_begin();
        for (u32 x = 0;
             x < BEJEWELED_GRID_COUNT;
             ++x) {
            for (u32 y = 0;
                 y < BEJEWELED_GRID_COUNT;
                 ++y) {
                Bejeweled_Slot *slot = &state->board.slots[x][y];
                if (!slot) continue;
                
                immediate_textured_quad(slot->center - slot->half_tile_size, slot->center + slot->half_tile_size, sheet->texture_id, slot->uv00, slot->uv10, slot->uv01, slot->uv11);
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
        
        // NOTE(diego): This is used to store dynamic possible_gems_for_slot in
        // possible_gems_for_slot routine.
        // Should be bigger enough to hold N_GEMS*GRID_COUNT*GRID_COUNT*sizeof(GEM)
        Memory_Index max_gems_memory = (((u32)(BejeweledGem_Count-1)) * BEJEWELED_GRID_COUNT*BEJEWELED_GRID_COUNT) * sizeof(Bejeweled_Gem);
        Memory_Index board_memory_size = max_gems_memory + sizeof(Bejeweled_State); // Make sure we have enough memory independently of Bejeweled_State size.
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
