internal void
clear_and_generate_board(Bejeweled_Board *board) {
    Bejeweled_State *state = board->state;
    Bejeweled_Level level = state->current_level;
    for (u32 x = 0; x < BEJEWELED_GRID_COUNT; ++x) {
        for (u32 y = 0; y < BEJEWELED_GRID_COUNT; ++y) {
            Bejeweled_Slot *slot = &board->slots[x][y];
            slot->chain_index = -1;
            slot->x = x;
            slot->y = y;
            if (level.data[x][y] == 1) {
                slot->type = BejeweledSlotType_Gem;
                slot->width = BEJEWELED_GEM_WIDTH;
                slot->height = BEJEWELED_GEM_HEIGHT;
                random_gem_for_slot(board, slot);
            } else {
                slot->type = BejeweledSlotType_None;
                slot->gem = BejeweledGem_None;
            }
        }
    }
}

internal void
clear_board(Bejeweled_Board *board) {
    Bejeweled_State *state = board->state;
    for (u32 x = 0; x < BEJEWELED_GRID_COUNT; ++x) {
        for (u32 y = 0; y < BEJEWELED_GRID_COUNT; ++y) {
            Bejeweled_Slot *slot = &board->slots[x][y];
            slot->type = BejeweledSlotType_None;
            slot->gem = BejeweledGem_None;
            slot->x = x;
            slot->y = y;
            slot->width = BEJEWELED_GEM_WIDTH;
            slot->height = BEJEWELED_GEM_HEIGHT;
            slot->chain_index = -1;
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
        BejeweledGem_Purple, // Donut
        BejeweledGem_Green, // Macaroon
        BejeweledGem_Orange, // SugarCookie
        BejeweledGem_Yellow, // Croissant
        BejeweledGem_Pink, // Cupcake
        BejeweledGem_Black, // Danish
        //BejeweledGem_White,
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
    
    real32 width  = BEJEWELED_GEM_WIDTH;
    real32 height = BEJEWELED_GEM_HEIGHT;
    real32 half_width  = width / 2.f;
    real32 half_height = height / 2.f;
    
    Vector2 start = get_start_xy(dim, width, height);
    
    swap->from_center = make_vector2(start.x + width * swap->from.x + half_width, start.y + height * swap->from.y + half_height);
    swap->to_center = make_vector2(start.x + width * swap->to.x + half_width, start.y + height * swap->to.y + half_height);
    swap->state = BejeweledSwapState_Prepared;
    
    swap->valid = is_swap_valid(&state->board, *swap);
    swap->reversing = false;
    
    state->swap = *swap;
}

internal void
reverse_swap(Bejeweled_Gem_Swap *swap) {
    Vector2 old_from_center = swap->from_center;
    swap->from_center = swap->to_center;
    swap->to_center = old_from_center;
    
    swap->state = BejeweledSwapState_Prepared;
    swap->valid = false;
    swap->reversing = true;
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
    swap_slots(board, *slot_a, *slot_b);
    clear_swap(&state->swap);
    
    handle_matches(state);
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
swap_slots(Bejeweled_Board *board, Bejeweled_Slot slot_a, Bejeweled_Slot slot_b) {
    Bejeweled_Slot a_copy = slot_a;
    Bejeweled_Slot b_copy = slot_b;
    
    s32 ax = a_copy.x;
    s32 ay = a_copy.y;
    
    s32 bx = b_copy.x;
    s32 by = b_copy.y;
    
    // Swap X's and Y's
    a_copy.x = bx;
    a_copy.y = by;
    //a_copy.center = b_copy.center;
    //a_copy.visual_center = b_copy.visual_center;
    
    b_copy.x = ax;
    b_copy.y = ay;
    //b_copy.center = a_copy.center;
    //b_copy.visual_center = a_copy.visual_center;
    
    board->slots[a_copy.x][a_copy.y] = a_copy;
    board->slots[b_copy.x][b_copy.y] = b_copy; 
}

internal b32
is_swap_possible(Bejeweled_Level *level, Bejeweled_Gem_Swap swap) {
    if (level->data[swap.from.x][swap.from.y] == 0) return false;
    if (level->data[swap.to.x][swap.to.y]     == 0) return false;
    
    u32 x_difference = abs(swap.to.x - swap.from.x);
    if (x_difference > 1) return false;
    
    u32 y_difference = abs(swap.to.y - swap.from.y);
    if (y_difference > 1) return false;
    
    if (x_difference == y_difference) return false;
    
    return true;
}

internal b32
is_swap_valid(Bejeweled_Board *board, Bejeweled_Gem_Swap swap) {
    Bejeweled_Slot *from = get_slot_at(board, swap.from.x, swap.from.y);
    Bejeweled_Slot *to = get_slot_at(board, swap.to.x, swap.to.y);
    
    Bejeweled_Gem old_from_gem = from->gem;
    
    // Fake swap just to check if there will be a chain.
    from->gem = to->gem;
    to->gem = old_from_gem;
    
    b32 result = has_chain(board, swap.to.x, swap.to.y);
    
    to->gem = from->gem;
    from->gem = old_from_gem;
    
    return result;
}

internal b32
is_tile_valid(Bejeweled_Level *level, Bejeweled_Tile tile) {
    if (tile.x < 0 || tile.x > BEJEWELED_GRID_COUNT) return false;
    if (tile.y < 0 || tile.y > BEJEWELED_GRID_COUNT) return false;
    
    if (level->data[tile.x][tile.y] == 0) return false;
    
    return true;
}

internal Bejeweled_Chain
make_chain(Bejeweled_Chain_Type type, s32 x, s32 y, s32 length) {
    assert(type > BejeweledChainType_None && type <= BejeweledChainType_Vertical);
    assert(x >= 0 && x < BEJEWELED_GRID_COUNT);
    assert(y >= 0 && y < BEJEWELED_GRID_COUNT);
    assert(length > 0 && length <= BEJEWELED_GRID_COUNT);
    
    Bejeweled_Chain result = {};
    result.type = type;
    result.x = x;
    result.y = y;
    result.length = length;
    
    return result;
}

internal Bejeweled_Chain_List
detect_horizontal_matches(Bejeweled_Board *board) {
    
    Bejeweled_Chain_List result = {};
    
    for (u32 y = 0;
         y < BEJEWELED_GRID_COUNT;
         ++y)
    {
        u32 x = 0;
        while (x < BEJEWELED_GRID_COUNT-2) {
            Bejeweled_Slot *slot = get_slot_at(board, x, y);
            if (slot && slot->gem != BejeweledGem_None) {
                Bejeweled_Gem match_gem = slot->gem;
                Bejeweled_Slot *second = get_slot_at(board, x + 1, y);
                Bejeweled_Slot *third  = get_slot_at(board, x + 2, y);
                if (second && third &&
                    second->gem == match_gem && third->gem == match_gem) {
                    
                    s32 start_x = x;
                    s32 start_y = y;
                    s32 length = 0;
                    
                    Bejeweled_Slot *next;
                    do {
                        length++; x++;
                        next = get_slot_at(board, x, y);
                    } while (x < BEJEWELED_GRID_COUNT && next && next->gem == match_gem);
                    
                    Bejeweled_Chain chain = make_chain(BejeweledChainType_Horizontal, start_x, start_y, length);
                    chain.index = sb_count(result.chains);
                    sb_push(result.chains, chain);
                    result.count++;
                }
            }
            x++;
        }
    }
    return result;
}

internal Bejeweled_Chain_List
detect_vertical_matches(Bejeweled_Board *board) {
    Bejeweled_Chain_List result = {};
    for (u32 x = 0;
         x< BEJEWELED_GRID_COUNT;
         ++x)
    {
        u32 y = 0;
        while (y < BEJEWELED_GRID_COUNT-2) {
            Bejeweled_Slot *slot = get_slot_at(board, x, y);
            if (slot && slot->gem != BejeweledGem_None) {
                Bejeweled_Gem match_gem = slot->gem;
                Bejeweled_Slot *second = get_slot_at(board, x, y + 1);
                Bejeweled_Slot *third  = get_slot_at(board, x, y + 2);
                if (second && third &&
                    second->gem == match_gem && third->gem == match_gem) {
                    
                    s32 start_x = x;
                    s32 start_y = y;
                    s32 length = 0;
                    
                    Bejeweled_Slot *next;
                    do {
                        length++; y++;
                        next = get_slot_at(board, x, y);
                    } while (y < BEJEWELED_GRID_COUNT && next && next->gem == match_gem);
                    
                    Bejeweled_Chain chain = make_chain(BejeweledChainType_Vertical, start_x, start_y, length);
                    chain.index = sb_count(result.chains);
                    sb_push(result.chains, chain);
                    
                    result.count++;
                }
            }
            y++;
        }
    }
    return result;
}

internal Bejeweled_Chain *
get_chain(Bejeweled_Chain_List *list, u32 index) {
    if (!list || !list->count) {
        int c = 0;
    }
    
    return &list->chains[index];
}

internal Bejeweled_Chain *
get_eating_chain(Bejeweled_State *state, Bejeweled_Slot *slot) {
    Bejeweled_Chain *result = 0;
    
    if (slot->chain_index >= 0) {
        result = get_chain(&state->matched_chains, slot->chain_index);
    }
    
    return result;
}


internal void
prepare_chain_for_eating(Bejeweled_Board *board, Bejeweled_Chain_List *list) {
    if (!list || !list->count) return;
    
    for (u32 chain_index = 0; chain_index < list->count; ++chain_index) {
        
        Bejeweled_Chain *chain = &list->chains[chain_index];
        chain->eating_t = 0.f;
        chain->eating_duration = .2f;
        
        switch (chain->type) {
            case BejeweledChainType_Vertical: {
                for (u32 y = 0; y < chain->length; ++y) {
                    Bejeweled_Slot *slot = get_slot_at(board, chain->x, chain->y + y);
                    slot->chain_index = chain->index;
                }
            } break;
            
            case BejeweledChainType_Horizontal: {
                for (u32 x = 0; x < chain->length; ++x) {
                    Bejeweled_Slot *slot = get_slot_at(board, chain->x + x, chain->y);
                    slot->chain_index = chain->index;
                }
            } break;
            
            invalid_default_case;
        }
    }
    
}

internal void
eat_chains(Bejeweled_State *state) {
    
    for (u32 chain_index = 0; chain_index < state->matched_chains.count; ++chain_index) {
        eat_chain(&state->board, &state->matched_chains.chains[chain_index]);
    }
    
    if (state->matched_chains.chains)
        sb_free(state->matched_chains.chains);
}

internal void
eat_chain(Bejeweled_Board *board, Bejeweled_Chain *chain) {
    assert(chain);
    
    if (chain->length < 3) return;
    
    if (chain->type == BejeweledChainType_Vertical) {
        for (u32 y = 0; y < chain->length; ++y) {
            Bejeweled_Slot *slot = get_slot_at(board, chain->x, chain->y + y);
            slot->gem = BejeweledGem_None;
            slot->width = BEJEWELED_GEM_WIDTH;
            slot->height = BEJEWELED_GEM_HEIGHT;
            slot->chain_index = -1; 
        }
        
    } else if (chain->type == BejeweledChainType_Horizontal) {
        for (u32 x = 0; x < chain->length; ++x) {
            Bejeweled_Slot *slot = get_slot_at(board, chain->x + x, chain->y);
            slot->gem = BejeweledGem_None;
            slot->width = BEJEWELED_GEM_WIDTH;
            slot->height = BEJEWELED_GEM_HEIGHT;
            slot->chain_index = -1;
        }
    }
}

internal b32
has_chain(Bejeweled_Board *board, u32 x, u32 y) {
    // Horizontal check
    Bejeweled_Slot slot = board->slots[x][y];
    
    s32 horizontal_length = 1;
    
    // Left
    s32 i = x - 1;
    while (i >= 0 && board->slots[i][y].gem == slot.gem) {
        Bejeweled_Slot *s = get_slot_at(board, i, y);
        if (!s || s->gem != slot.gem) break;
        
        i--;
        horizontal_length++;
    }
    
    // Right
    i = x + 1;
    while (i < BEJEWELED_GRID_COUNT) {
        Bejeweled_Slot *s = get_slot_at(board, i, y);
        if (!s || s->gem != slot.gem) break;
        
        i++;
        horizontal_length++;
    }
    
    if (horizontal_length >= 3) return true;
    
    // Vertical check
    s32 vertical_length = 1;
    
    // Top
    i = y - 1;
    while (i >= 0) {
        Bejeweled_Slot *s = get_slot_at(board, x, i);
        if (!s || s->gem != slot.gem) break;
        
        i--;
        vertical_length++;
    }
    
    // Bottom
    i = y + 1;
    while (i < BEJEWELED_GRID_COUNT) {
        Bejeweled_Slot *s = get_slot_at(board, x, i);
        if (!s || s->gem != slot.gem) break;
        
        i++;
        vertical_length++;
    }
    
    return vertical_length >= 3;
}

internal Bejeweled_Sprite_UV
get_uvs_for_sprite(Spritesheet *sheet, Sprite *sprite) {
    assert(sheet && sprite);
    
    Bejeweled_Sprite_UV result = {};
    
    real32 u0 = sprite->x / (real32) sheet->width;
    real32 u1 = (sprite->x + sprite->width) / (real32) sheet->width;
    
    // Flip UVs
    real32 v0 = 1.f - (sprite->y / (real32) sheet->height);
    real32 v1 = 1.f - ((sprite->y + sprite->height) / (real32) sheet->height);
    
    result._00 = make_vector2(u0, v0);
    result._10 = make_vector2(u1, v0);
    result._01 = make_vector2(u0, v1);
    result._11 = make_vector2(u1, v1);
    
    return result;
}

internal Bejeweled_Gem
get_random_gem(Bejeweled_State *state) {
    u32 random_slot = random_int_in_range(0, array_count(state->gems) - 1);
    return state->gems[random_slot].gem;
}

internal Sprite *
get_sprite(Bejeweled_State *state, Bejeweled_Gem gem, b32 highlighted = false) {
    Bejeweled_Gem_Sprite *sprites = highlighted ? state->highlighted_gems : state->gems;
    Bejeweled_Gem_Sprite *g = &sprites[(u32) gem - 1];
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
    if ((s32)slot->x == x && (s32)slot->y == y) {
        
    } else {
        int c = 0;
    }
    
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
get_tile_under_xy(Bejeweled_State *state, s32 x, s32 y) {
    Bejeweled_Tile result = {-1, -1};
    
    real32 slot_width = BEJEWELED_GEM_WIDTH;
    real32 slot_height = BEJEWELED_GEM_HEIGHT;
    
    Vector2 start = get_start_xy(state->memory->window_dimensions, slot_width, slot_height);
    
    real32 max_x = start.x + slot_width  * BEJEWELED_GRID_COUNT;
    real32 max_y = start.y + slot_height * BEJEWELED_GRID_COUNT;
    
    if (x < start.x || x > max_x) return result;
    if (y < start.y || y > max_y) return result;
    
    s32 tile_x = (s32) ((x - start.x) / slot_width);
    s32 tile_y = (s32) ((y - start.y) / slot_height);
    
    if (tile_x >= 0 && tile_x < BEJEWELED_GRID_COUNT &&
        tile_y >= 0 && tile_y < BEJEWELED_GRID_COUNT) {
        result.x = tile_x;
        result.y = tile_y;
    }
    
    return result;
}

internal Vector2
get_start_xy(Vector2i dim, real32 width, real32 height) {
    width  *=  BEJEWELED_GRID_COUNT;
    height *= BEJEWELED_GRID_COUNT;
    
    real32 sx = center_horizontally((real32) dim.width, width);
    real32 sy = center_vertically((real32) dim.height, height);
    
    return make_vector2(sx, sy);
}

internal Bejeweled_Tile *
get_dropping_tiles(Bejeweled_State *state) {
    
    Bejeweled_Tile *result = 0;
    
    for (u32 y = BEJEWELED_GRID_COUNT - 1;
         y >= 0;
         y--)
    {
        for (u32 x = 0;
             x < BEJEWELED_GRID_COUNT;
             x++)
        {
            Bejeweled_Slot *slot = get_slot_at(&state->board, x, y);
            if (slot->type == BejeweledSlotType_Gem && slot->gem == BejeweledGem_None)
            {
                Bejeweled_Tile tile = {};
                tile.x = x;
                tile.y = y;
                sb_push(result, tile);
            }
        }
        if (sb_count(result) > 0) break;
    }
    
    return result;
}

internal Bejeweled_Level 
load_level(char *levelname) {
    Bejeweled_Level result = {};
    
    char *folder = "./data/levels/bejeweled/";
    char *lvl = ".lvl";
    
    char *filename = concat(levelname, lvl, string_length(levelname), string_length(lvl));
    char *filepath = concat(folder, filename, string_length(folder), string_length(filename));
    
    assert(string_length(filepath) != 0);
    assert(string_ends_with(filepath, ".lvl"));
    
    File_Contents level_file = platform_read_entire_file(filepath);
    assert(level_file.file_size > 0);
    
    Text_File_Handler handler = {};
    init_handler(&handler, level_file.contents);
    
    assert(handler.num_lines > 0);
    
    u32 row = 0;
    while (row < handler.num_lines) {
        char *line = consume_next_line(&handler);
        if (!line) break;
        
        u32 length = string_length(line);
        assert(length == BEJEWELED_GRID_COUNT); // @NOTE(diego): We only support levels with this size.
        for (u32 char_index = 0; char_index < length; ++char_index) {
            u32 value = 0;
            if (line[char_index] == '1') {
                value = 1;
            }
            result.data[char_index][row] = value;
        }
        ++row;
    }
    
    platform_free(handler.data);
    platform_free(filepath);
    platform_free(filename);
    
    return result;
}

internal void
init_game(Bejeweled_State *state) {
    state->game_mode = GameMode_Playing;
    state->memory->game_mode = GameMode_Playing;
    state->board.state = state;
    state->swap = {};
    
    state->current_level = load_level("bejeweled_02");
    
#define BEJEWELED_GEM(GEM_TYPE) state->gems[GEM_TYPE-1].gem = GEM_TYPE; \
state->highlighted_gems[GEM_TYPE-1].gem = GEM_TYPE
    
    BEJEWELED_GEM(BejeweledGem_Purple);
    BEJEWELED_GEM(BejeweledGem_Green);
    BEJEWELED_GEM(BejeweledGem_Orange);
    BEJEWELED_GEM(BejeweledGem_Yellow);
    BEJEWELED_GEM(BejeweledGem_Pink);
    BEJEWELED_GEM(BejeweledGem_Black);
    //BEJEWELED_GEM(BejeweledGem_White);
    
    Spritesheet *sheet = state->assets.main_sheet;
    for (u32 sprite_index = 0;
         sprite_index < sheet->num_sprites;
         ++sprite_index)
    {
        Sprite *spr = get_sprite(sheet, sprite_index);
        if (spr && spr->name) {
            if (string_starts_with("gem", spr->name)) {
                u32 gem_index = atoi(&spr->name[3]) - 1;
                Bejeweled_Gem_Sprite *gem_s = &state->gems[gem_index];
                gem_s->sprite = spr;
                
                state->assets.gem_uvs[gem_index] = get_uvs_for_sprite(sheet, spr);
            } else if (string_starts_with("h_gem", spr->name)) {
                u32 gem_index = atoi(&spr->name[5]) - 1;
                Bejeweled_Gem_Sprite *gem_s = &state->highlighted_gems[gem_index];
                gem_s->sprite = spr;
                state->assets.highlighted_gem_uvs[gem_index] = get_uvs_for_sprite(sheet, spr);
            } else if (strings_are_equal("tile", spr->name)) {
                state->assets.tile_sprite = spr;
                // TODO(diego): Move to gt_sprite
                real32 u0 = spr->x / (real32) sheet->width;
                real32 u1 = (spr->x + spr->width) / (real32) sheet->width;
                // Flip UVs
                real32 v0 = 1.f - (spr->y / (real32) sheet->height);
                real32 v1 = 1.f - ((spr->y + spr->height) / (real32) sheet->height);
                state->assets.tile_uv = Bejeweled_Sprite_UV{make_vector2(u0, v0), make_vector2(u1, v0), make_vector2(u0, v1),
                    make_vector2(u1, v1)};
            }
        }
    }
    
    platform_show_cursor(true);
    
    clear_and_generate_board(&state->board);
    
    if (!state->background_music) {
        state->background_music = play_sound("Moonlight", &state->music);
    }
    set_volume(state->background_music, .1f);
}

internal void
update_game(Bejeweled_State *state, Game_Input *input) {
    handle_mouse(state, input);
    handle_swap(state);
    handle_chains(state);
}

internal void
handle_mouse(Bejeweled_State *state, Game_Input *input) {
    platform_get_cursor_position(&state->mouse_position);
    
    state->highlighted_tile = Bejeweled_Tile{-1, -1};
    if (state->swap.state == BejeweledSwapState_Swapping || state->swap.state == BejeweledSwapState_Prepared) {
        return;
    }
    
    state->highlighted_tile = get_tile_under_xy(state, state->mouse_position.x, state->mouse_position.y);
    
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
        if (!is_tile_valid(&state->current_level, tile)) return;
        
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
        
        if (is_swap_possible(&state->current_level, swap)) {
            prepare_swap(state, &swap);
        }
        
    }
}

internal Bejeweled_Chain_List
combine_chain_lists(Bejeweled_Chain_List first, Bejeweled_Chain_List second) {
    Bejeweled_Chain_List combined = {};
    
    for (u32 chain_index = 0; chain_index < first.count; ++chain_index) {
        sb_push(combined.chains, first.chains[chain_index]);
    }
    for (u32 chain_index = 0; chain_index < second.count; ++chain_index) {
        sb_push(combined.chains, second.chains[chain_index]);
    }
    combined.count = first.count + second.count;
    
    sb_free(first.chains);
    sb_free(second.chains);
    
    return combined;
}

internal b32
handle_chain_list(Bejeweled_State *state, Bejeweled_Chain_List *list) {
    if (!list || !list->count) return false;
    
    for (u32 chain_index = 0; chain_index < list->count; ++chain_index) {
        Bejeweled_Chain *chain = &list->chains[chain_index];
        if (chain->eating_t >= chain->eating_duration) {
            eat_chain(&state->board, chain);
            list->count--;
        }
        chain->eating_t += core.time_info.dt;
    }
    // NOTE(diego): All chains will get to eat_chain at the same time!
    b32 handled = list->count == 0;
    
    if (handled) sb_free(state->matched_chains.chains);
    
    return handled;
}

internal void
handle_chains(Bejeweled_State *state) {
    if (handle_chain_list(state, &state->matched_chains)) {
        // TODO: Implement falling gems
    }
}

internal void
handle_swap(Bejeweled_State *state) {
    
    Bejeweled_Gem_Swap *s = &state->swap;
    
    switch (s->state) {
        case BejeweledSwapState_Prepared: {
            start_swap(state, s);
            if (s->valid) {
                play_sound("Swap", &state->swap_sound, false);
            } else if (s->reversing) {
                play_sound("Error", &state->invalid_swap_sound, false);
            }
        } break;
        
        case BejeweledSwapState_Swapping: {
            if (s->t >= s->duration) {
                if (s->valid) {
                    do_swap(state, s);
                } else if (!s->reversing) {
                    reverse_swap(s);
                } else {
                    clear_swap(s);
                }
                return;
            }
            s->t += core.time_info.dt;
            
        } break;
        
        default: {
            // NOTE(diego): Ignored.
        } break;
    }
}

internal void
handle_matches(Bejeweled_State *state) {
    Bejeweled_Chain_List horizontal_chains = detect_horizontal_matches(&state->board);
    Bejeweled_Chain_List vertical_chains = detect_vertical_matches(&state->board);
    
    state->matched_chains = combine_chain_lists(horizontal_chains, vertical_chains);
    if (!state->matched_chains.count) {
        // TODO(diego): Move to next turn, which is enable user to interact with the game if there's possible swaps
        // otherwise shuffle until we have possible swaps
        return;
    }
    
    b32 animated = true;
    if (!animated) {
        eat_chains(state);
    } else {
        Bejeweled_Board *b = &state->board;
        prepare_chain_for_eating(b, &state->matched_chains);
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
        
        Bejeweled_Assets assets = state->assets;
        Spritesheet *sheet = assets.main_sheet;
        
        Vector2 start = get_start_xy(dim, BEJEWELED_GEM_WIDTH, BEJEWELED_GEM_HEIGHT);
        Bejeweled_Gem_Swap *s = &state->swap;
        
        immediate_begin();
        for (s32 x = 0;
             x < BEJEWELED_GRID_COUNT;
             ++x) {
            for (s32 y = 0;
                 y < BEJEWELED_GRID_COUNT;
                 ++y) {
                Bejeweled_Slot *slot = &state->board.slots[x][y];
                if (!slot || slot->type == BejeweledSlotType_None) continue;
                
                Bejeweled_Chain *eating_chain = get_eating_chain(state, slot);
                if (eating_chain && eating_chain->eating_t <= eating_chain->eating_duration) {
                    real32 t = smooth_start3(1.f - clampf(0.f, eating_chain->eating_t / eating_chain->eating_duration, 1.f));
                    slot->width  = BEJEWELED_GEM_WIDTH*t;
                    slot->height = BEJEWELED_GEM_HEIGHT*t;
                }
                
                // @NOTE(diego): This can be hardcoded to BEJEWELED_GEM_WIDTH and BEJEWELED_GEM_HEIGHT
                real32 hw = slot->width  / 2.f;
                real32 hh = slot->height / 2.f;
                
                real32 thw = BEJEWELED_GEM_WIDTH  / 2.f;
                real32 thh = BEJEWELED_GEM_HEIGHT / 2.f;
                
                Vector2 center = make_vector2(start.x + BEJEWELED_GEM_WIDTH * x + thw, start.y + BEJEWELED_GEM_HEIGHT * y + thh);
                Vector2 tile_center = center;
                
                // Make sure our 'from' Gem is in front of the 'to' one
                real32 z_index = 0.8f;
                if (s->state == BejeweledSwapState_Swapping && s->t < s->duration) {
                    real32 t = clampf(0.f, s->t / s->duration, 1.f);
                    if (s->from.x == x && s->from.y == y) {
                        center = lerp_vector2(s->from_center, t, s->to_center);
                        z_index -= 0.01f;
                    } else if (s->to.x == x && s->to.y == y) {
                        center = lerp_vector2(s->to_center, t, s->from_center);
                    }
                }
                
                if (slot->gem != BejeweledGem_None) {
                    u32 index = ((u32) slot->gem) - 1;
                    
                    Bejeweled_Sprite_UV uvs = state->assets.gem_uvs[index];
                    if (state->highlighted_tile.x == x && state->highlighted_tile.y == y) {
                        uvs = state->assets.highlighted_gem_uvs[index];
                    }
                    
                    Vector2 gem_min = make_vector2(center.x - hw, center.y - hh);
                    Vector2 gem_max = make_vector2(center.x + hw, center.y + hh);
                    immediate_textured_quad(gem_min, gem_max, sheet->texture_id, uvs._00, uvs._10, uvs._01, uvs._11, z_index);
                }
                
                Vector2 tile_min = make_vector2(tile_center.x - thw, tile_center.y - thh);
                Vector2 tile_max = make_vector2(tile_center.x + thw, tile_center.y + thh);
                immediate_textured_quad(tile_min, tile_max, sheet->texture_id, assets.tile_uv._00, assets.tile_uv._10, assets.tile_uv._01, assets.tile_uv._11, 0.9f);
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
        
        state->swap_sound = load_sound("./data/sounds/bejeweled/swap.wav");
        state->invalid_swap_sound = load_sound("./data/sounds/bejeweled/error.wav");
        state->music = load_sound("./data/sounds/bejeweled/Mining by Moonlight.wav");
        
        Bejeweled_Assets assets = {};
        Spritesheet *s = load_spritesheet("./data/textures/bejeweled/cookie/sprites.txt");
        assets.main_sheet = s;
        state->assets = assets;
        
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
    
    if (state->background_music) state->background_music->flags &= ~PLAYING_SOUND_ACTIVE;
}
