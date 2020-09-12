
internal memory_card
get_random_card() {
    memory_card result = {};
    
    while (result.kind == MemoryCard_None) {
        if (random_choice(1)) {
            result.kind = MemoryCard_Donut;
        }
        
        if (random_choice(2)) {
            result.kind = MemoryCard_Square;
        }
        
        if (random_choice(3)) {
            result.kind = MemoryCard_Eye;
        }
        
        if (random_choice(4)) {
            result.kind = MemoryCard_Lines;
        }
    }
    result.color = make_v4(random_real32_in_range(0.f, 1.f), random_real32_in_range(0.f, 1.f), random_real32_in_range(0.f, 1.f), 1.f);
    
    return result;
}

// NOTE(diego): 


#define memory_half_puzzle(a, b) (((sizeof(a) / sizeof((a)[0])) * (sizeof(b) / sizeof((b)[0]))) / 2)

internal void
init_world(memory_puzzle_state *state) {
    state->world = {}; // Ensure zero
    
    int x_count = array_count(state->world.field);
    int y_count = array_count(state->world.field[0]);
    
    for (int i = 0; i < memory_half_puzzle(state->world.field, state->world.field[0]); ++i) {
        
        memory_card new_card = get_random_card();
        int placed = 0;
        while (placed < 2) {
            int field_x = random_int_in_range(0, x_count);
            int field_y = random_int_in_range(0, y_count);
            
            memory_card card_at_xy = state->world.field[field_y][field_x];
            if (card_at_xy.kind == MemoryCard_None) {
                state->world.field[field_y][field_x] = new_card;
                ++placed;
            } else {
                // Find first available space and put it there
                // NOTE(diego): This is pretty bad because the last card can end up side by side
                // but it just for training. Not a commercial game by any means!!
                memory_card *cards = (memory_card *) state->world.field;
                for (memory_card *card = cards; card != cards + (x_count * y_count); card++) {
                    if (card->kind == MemoryCard_None) {
                        *card = new_card;
                        ++placed; 
                        break;
                    }
                }
            }
        }
    }
    
}

internal memory_card *
get_current_selected_card(memory_puzzle_state *state) {
    int field_x = state->current_selected % array_count(state->world.field);
    int field_y = state->current_selected / array_count(state->world.field);
    
    return &state->world.field[field_x][field_y];
}

internal b32
flipped_cards_match(memory_card *first, memory_card *second) {
    if (!first || !second) return false;
    if (!first->flipped || !second->flipped) return false;
    
    return (first->kind == second->kind && 
            equal_v4(first->color, second->color)); 
}

internal void
draw_square(memory_card *card, v2 min, v2 max) {
    v2 square_size = mul_v2(sub_v2(max, min), 0.25f); 
    
    v2 square_min = add_v2(min, square_size);
    v2 square_max = sub_v2(max, square_size);
    
    immediate_quad(square_min, square_max, card->color, 1.f);
}

internal void
draw_eye(memory_card *card, v2 min, v2 max) {
    v2 radius = sub_v2(max, min);
    v2 center = make_v2((min.x + max.x) / 2.f, (min.y + max.y) / 2.f);
    immediate_circle_filled(center, make_v2(radius.x * 0.6f, radius.y * 0.3f), card->color);
}

internal void
draw_donut(memory_card *card, v2 min, v2 max) {
    v2 radius = sub_v2(max, min);
    v2 center = make_v2((min.x + max.x) / 2.f, (min.y + max.y) / 2.f);
    immediate_circle(center, radius.x * 0.15f, radius.y * 0.4f, card->color);
}

internal void
draw_lines(memory_card *card, v2 min, v2 max) {
    
    int line_count = 10;
    
    real32 height = max.y - min.y;
    real32 line_height = 1.f;
    
    real32 advance_y = (height + line_count * line_height) / line_count;
    real32 y_cursor = min.y;
    
    for (int y = 0; y < line_count - 1; y++) {
        
        v2 line_min = make_v2(min.x, y_cursor);
        v2 line_max = make_v2(max.x, y_cursor + line_height);
        immediate_quad(line_min, line_max, card->color, 1.f);
        
        y_cursor += advance_y;
    }
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
                
                case MemoryCard_Eye: {
                    draw_eye(&card, min, max);
                } break;
                case MemoryCard_Donut: {
                    draw_donut(&card, min, max);
                } break;
                case MemoryCard_Square: {
                    draw_square(&card, min, max);
                } break;
                case MemoryCard_Lines: {
                    draw_lines(&card, min, max);
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
        state->checking_cards_target = 1.f;
        
        memory->initialized = true;
    }
    
    state->world.dimensions = memory->window_dimensions;
    
    //
    // Update
    //
    
    real32 dt = memory->dt;
    
    if (state->game_mode == GameMode_Playing) {
        //
        // Update field
        //
        
        if (pressed(Button_Left)) state->current_selected--;
        if (pressed(Button_Right)) state->current_selected++;
        if (pressed(Button_Up)) state->current_selected -= array_count(state->world.field);
        if (pressed(Button_Down)) state->current_selected += array_count(state->world.field);
        
        state->current_selected = (s8) clamp(0, state->current_selected, array_count(state->world.field) * array_count(state->world.field[0]) - 1);
        
        
        //
        // If we are checking for equal cards then
        //
        if (state->checking_cards_t > 0.f) {
            state->checking_cards_t += dt;
            
            if (state->checking_cards_t > state->checking_cards_target) {
                state->checking_cards_t = .0f;
                
                memory_card *first = state->first_flipped;
                memory_card *second = state->second_flipped;
                if (flipped_cards_match(first, second)) {
                    // TODO(diego): Increment highscore!!
                } else {
                    first->flipped = false;
                    second->flipped = false;
                }
                state->first_flipped = 0;
                state->second_flipped = 0;
            }
            
        } else {
            if (pressed(Button_Space)) {
                memory_card *card = get_current_selected_card(state);
                if (card && !card->flipped) {
                    card->flipped = !card->flipped;
                    if (!state->first_flipped) {
                        state->first_flipped = card;
                    } else if (!state->second_flipped) {
                        state->second_flipped = card;
                    }
                    
                    if (state->first_flipped && state->second_flipped) {
                        state->checking_cards_t = dt;
                    }
                }
            }
        }
    }
    
    draw_game_view(state);
}