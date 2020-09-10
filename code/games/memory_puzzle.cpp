
internal void
init_world(memory_puzzle_state *state) {
    state->world = {}; // Ensure zero
    
    state->world.field[0][1].flipped = true;
    state->world.field[0][1].kind = MemoryCard_Square;
    state->world.field[0][1].color = make_color(0xffffff00);
}

internal void
draw_square(memory_card *card, v2 min, v2 max) {
    v2 square_size = mul_v2(sub_v2(max, min), 0.25f); 
    
    v2 square_min = add_v2(min, square_size);
    v2 square_max = sub_v2(max, square_size);
    
    immediate_quad(square_min, square_max, card->color, 1.f);
}

internal memory_card *
get_current_selected_card(memory_puzzle_state *state) {
    int field_x = state->current_selected % array_count(state->world.field);
    int field_y = state->current_selected / array_count(state->world.field);
    
    return &state->world.field[field_x][field_y];
}

internal void
draw_game_view(memory_puzzle_state *state) {
    //
    // Draw field
    //
    
    memory_puzzle_world world = state->world;
    
    real32 pad;
    if (world.dimensions.width > world.dimensions.height) {
        pad = world.dimensions.height * .025f;
    } else {
        pad = world.dimensions.width * .025f;
    }
    
    real32 x_count = (real32) array_count(world.field);
    real32 y_count = (real32) array_count(world.field[0]);
    
    real32 max_height = world.dimensions.height * 0.8f;
    real32 card_size = (max_height - y_count * pad) / y_count;
    
    v2 start = make_v2((world.dimensions.width - card_size * x_count) / 2.f, (world.dimensions.height - card_size * y_count) / 2.f);
    immediate_begin();
    for (int field_y = 0; field_y < array_count(world.field[0]); ++field_y) {
        for (int field_x = 0; field_x < array_count(world.field); ++field_x) {
            v2 min = make_v2(card_size * field_x + pad, card_size * field_y + pad);
            v2 max = make_v2(card_size * field_x + card_size, field_y * card_size + card_size);
            
            min = add_v2(min, start);
            max = add_v2(max, start);
            
            memory_card card = world.field[field_x][field_y];
            if (!card.flipped) {
                v4 white = make_color(0xffffffff);
                immediate_quad(min, max, white, 1.f);
                continue;
            }
            
            switch (card.kind) {
                
                case MemoryCard_Square: {
                    draw_square(&card, min, max);
                } break;
                
                default: break;
            }
        }
    }
    
    //
    // Draw selected indicator
    //
    {
        int field_x = state->current_selected % array_count(world.field);
        int field_y = state->current_selected / array_count(world.field);
        
        v2 min = make_v2(card_size * field_x, card_size * field_y);
        v2 max = make_v2(card_size * field_x + card_size, field_y * card_size + card_size);
        
        min = add_v2(min, start);
        max = add_v2(max, start);
        
        
        real32 stroke = pad * .5f;
        real32 half_stroke = stroke * .5f;
        
        min.x += stroke;
        max.x += stroke;
        
        min.y += stroke;
        max.y += stroke;
        
        v4 color = make_color(0xff0000ff);
        
        v2 left_min = make_v2(min.x, min.y);
        v2 left_max = make_v2(min.x + half_stroke, max.y);
        
        v2 right_min = make_v2(max.x, min.y);
        v2 right_max = make_v2(max.x - half_stroke, max.y);
        
        v2 top_min = make_v2(min.x, min.y);
        v2 top_max = make_v2(max.x, min.y + half_stroke);
        
        v2 bottom_min = make_v2(min.x, max.y);
        v2 bottom_max = make_v2(max.x, max.y - half_stroke);
        
        immediate_quad(left_min, left_max ,color, 1.f);
        immediate_quad(right_min, right_max, color, 1.f);
        immediate_quad(top_min, top_max, color, 1.f);
        immediate_quad(bottom_min, bottom_max, color, 1.f);
    }
    
    immediate_flush();
}

internal void
draw_game_over(memory_puzzle_state *state) {
    
}


internal void
memory_puzzle_game_update_and_render(game_memory *memory, game_input *input) {
    
    memory_puzzle_state *state = (memory_puzzle_state *) memory->permanent_storage;
    if (!memory->initialized) {
        assert(memory->permanent_storage_size == 0);
        assert(!memory->permanent_storage);
        
        state = (memory_puzzle_state *) game_alloc(memory, megabytes(12));
        
        init_world(state);
        
        state->game_mode = GameMode_Playing;
        
        memory->initialized = true;
    }
    
    state->world.dimensions = memory->window_dimensions;
    
    //
    // Update
    //
    if (state->game_mode == GameMode_Playing) {
        
        //
        // Update field
        //
        if (pressed(Button_Left)) state->current_selected--;
        if (pressed(Button_Right)) state->current_selected++;
        if (pressed(Button_Up)) state->current_selected -= array_count(state->world.field);
        if (pressed(Button_Down)) state->current_selected += array_count(state->world.field);
        
        state->current_selected = (s8) clamp(0, state->current_selected, array_count(state->world.field) * array_count(state->world.field[0]) - 1);
        
        
        if (pressed(Button_Space)) {
            memory_card *card = get_current_selected_card(state);
            if (card) {
                card->flipped = !card->flipped;
                if (state->last_flipped) {
                    // TODO(diego): Compare cards
                    state->last_flipped = 0;
                } else {
                    state->last_flipped = card;
                }
            }
        }
        
    }
    
    draw_game_view(state);
}