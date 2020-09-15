
internal void
init_level(memory_puzzle_state *state) {
    state->current_level = {};
    state->current_level.max_tries = 100;
}

internal void
memory_puzzle_game_restart(memory_puzzle_state *state) {
    state->game_mode = GameMode_Playing;
    state->flip_count = 0;
    state->quit_was_selected = false;
    state->menu_selected_item = 0;
    
    //
    // Re-init
    //
    state->world = init_world();
    
    init_level(state);
}


internal v4
get_random_card_color() {
    real32 t = random_real32_in_range(0.f, 2.f);
    v4 result = {};
    
    if (t >= 2.f) {
        result = make_color(0xffc19c8e); 
    } else if (t >= 1.9f) {
        result = make_color(0xff829094);
    } else if (t >= 1.8f) {
        result = make_color(0xff92c379);
    } else if (t >= 1.6f) {
        result = make_color(0xff55a6dd);
    } else if (t >= 1.5f) {
        result = make_color(0xff9559f9);
    } else if (t >= 1.4f) {
        result = make_color(0xff00ba04);
    } else if (t >= 1.3f) {
        result = make_color(0xfff94836);
    } else if (t >= 1.2f) {
        result = make_color(0xff160fc7);
    } else if (t >= 1.1f) {
        result = make_color(0xff21d891);
    } else if (t >= 1.0f) {
        result = make_color(0xffbae87f);
    } else if (t >= .9f) {
        result = make_color(0xff41784d);
    } else if (t >= .8f) {
        result = make_color(0xff3f849f);
    } else if (t >= .7f) {
        result = make_color(0xffffc731);
    } else if (t >= .6f) {
        result = make_color(0xff463b43);
    } else if (t >= .5f) {
        result = make_color(0xffef8f2d);
    } else if (t >= .4f) {
        result = make_color(0xffaec6c7);
    } else if (t >= .3f) {
        result = make_color(0xff3f849f);
    } else if (t >= .2f) {
        result = make_color(0xff9559f9);
    } else if (t >= .1f) {
        result = make_color(0xff5d3c26);
    } else if (t >= .0f) {
        result = make_color(0xffffffff);
    }
    return result;
}

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
            result.kind = MemoryCard_DoubleSquare;
        }
        
        if (random_choice(4)) {
            result.kind = MemoryCard_Lines;
        }
        if (random_choice(5)) {
            result.kind = MemoryCard_Oval;
        }
        if (random_choice(6)) {
            result.kind = MemoryCard_Circle;
        }
        if (random_choice(7)) {
            result.kind = MemoryCard_Diamond;
        }
    }
    
    result.color = get_random_card_color();
    
    return result;
}

internal memory_puzzle_world
init_world() {
    memory_puzzle_world world = {}; // Ensure zero
    
    int x_count = array_count(world.field);
    int y_count = array_count(world.field[0]);
    int size = x_count * y_count;
    
    memory_card *cards = (memory_card *) world.field;
    for (memory_card *card = cards; card != cards + size;) {
        memory_card new_card = get_random_card();
        
        b32 contains = false;
        for (memory_card *c = cards; c != cards + size; c++) {
            if (c->kind == new_card.kind && 
                equal_v4(c->color, new_card.color)) {
                contains = true;
                break;
            }
        }
        if (!contains) {
            *card++ = new_card;
            *card++ = new_card;
        }
    }
    
    //
    // Shuffle using Fisher–Yates algorithm 
    //
    // for i from n−1 downto 1 do
    //     j ← random integer such that 0 ≤ j ≤ i
    //     exchange a[j] and a[i]
    //
    for (int i = size - 1; i >= 0; --i) {
        int j = random_int_in_range(0, size - 1);
        memory_card *first = cards + i;
        memory_card *second = cards + j;
        memory_card aux = *first;
        *first = *second;
        *second = aux;
    }
    
    return world;
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
draw_oval(memory_card *card, v2 min, v2 max) {
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
    
    real32 height = max.y - min.y;
    real32 line_height = 1.f;
    
    real32 line_count = 24.f;
    
    real32 advance_y = (height * line_height) / (line_count - 1);
    real32 y_cursor = min.y;
    
#if 0
    v2 default_uv = make_v2(1.f, 1.f);
    real32 angle = -45.f;
#endif
    
    for (int y = 0; y < line_count - 1; y++) {
        
        // STUDY(diego): How to clip this properly to max min sizes...
#if 0
        v2 center = make_v2((min.x + max.x) / 2.f, (min.y + max.y) / 2.f);
        
        v2 top_left = rotate_v2_around(make_v2(min.x, y_cursor), center, angle);
        v2 top_right = rotate_v2_around(make_v2(max.x, y_cursor), center, angle);
        v2 bottom_left = rotate_v2_around(make_v2(min.x, y_cursor + line_height), center, angle);
        v2 bottom_right = rotate_v2_around(make_v2(max.x, y_cursor + line_height), center, angle);
        
        immediate_vertex(top_left, card->color, default_uv, 1.f);
        immediate_vertex(bottom_left, card->color, default_uv, 1.f);
        immediate_vertex(bottom_right, card->color, default_uv, 1.f);
        
        immediate_vertex(top_right, card->color, default_uv, 1.f);
        immediate_vertex(bottom_right, card->color, default_uv, 1.f);
        immediate_vertex(top_left, card->color, default_uv, 1.f);
#endif
        
        v2 line_min = make_v2(min.x, y_cursor);
        v2 line_max = make_v2(max.x, y_cursor + line_height);
        immediate_quad(line_min, line_max, card->color, 1.f);
        
        y_cursor += advance_y;
    }
}

internal void
draw_double_square(memory_card *card, v2 min, v2 max) {
    v2 size = mul_v2(sub_v2(max, min), 0.5f); 
    v2 first_min = min;
    v2 first_max = add_v2(first_min, size);
    
    v2 second_min = first_max;
    v2 second_max = add_v2(second_min, size);
    
    immediate_quad(first_min, first_max, card->color, 1.f);
    immediate_quad(second_min, second_max, card->color, 1.f);
}

internal void
draw_circle(memory_card *card, v2 min, v2 max) {
    v2 radius = mul_v2(sub_v2(max, min), 0.5f);
    v2 center = make_v2((min.x + max.x) / 2.f, (min.y + max.y) / 2.f);
    immediate_circle_filled(center, radius, card->color);
}

internal void
draw_diamond(memory_card *card, v2 min, v2 max) {
    v2 half = make_v2((min.x + max.x) / 2.f, (min.y + max.y) / 2.f);
    v2 default_uv = make_v2(-1.f, -1.f);
    
    // First triangle
    immediate_vertex(make_v2(half.x, min.y), card->color, default_uv, 1.f);
    immediate_vertex(make_v2(min.x, half.y), card->color, default_uv, 1.f);
    immediate_vertex(make_v2(half.x, max.y), card->color, default_uv, 1.f);
    
    // Second triangle
    immediate_vertex(make_v2(half.x, min.y), card->color, default_uv, 1.f);
    immediate_vertex(make_v2(half.x, max.y), card->color, default_uv, 1.f);
    immediate_vertex(make_v2(max.x, half.y), card->color, default_uv, 1.f);
}

internal void
memory_puzzle_menu_art(v2 min, v2 max) {
    v4 background = make_color(0xff141414);
    
    immediate_begin();
    immediate_quad(min, max, background, 1.f);
    
    immediate_flush();
}

internal void
draw_menu(memory_puzzle_state *state) {
    v2i dim = state->world.dimensions;
    
    v4 white          = make_color(0xffffffff);
    v4 default_color  = make_color(0xffaaaaaa);
    v4 selected_color = make_color(0xffffff00);
    
    char *menu_title = state->game_mode == GameMode_GameOver ? "Game Over" : "Memory Puzzle";
    real32 menu_title_width = get_text_width(&state->assets.menu_title_font, menu_title);
    
    real32 y = dim.height * 0.2f;
    
    //
    // Title
    //
    //  Retry
    //  Quit
    //
    
    draw_text((dim.width - menu_title_width) / 2.f, y, (u8 *) menu_title, &state->assets.menu_title_font, white);
    
    char* menu_items[] = {"Retry", state->quit_was_selected ? "Quit? Are you sure?" : "Quit"};
    
    y += dim.height * 0.25f;
    for (int menu_item = 0; menu_item < array_count(menu_items); ++menu_item) {
        
        char *text = menu_items[menu_item];
        real32 width = get_text_width(&state->assets.menu_item_font, text);
        
        if (state->menu_selected_item == menu_item) {
            v4 non_white = make_color(0xffffde00);
            
            real32 now = cosf(time_info.current_time);
            real32 t = cosf(now * 3);
            t *= t;
            
            t = .4f + .54f * t;
            v4 front_color = lerp_color(non_white, t, white);
            
            // Also draw an extra background item.
            real32 offset = state->assets.menu_item_font.line_height / 40;
            draw_text((dim.width - width) / 2.f, y, (u8 *) text, &state->assets.menu_item_font, selected_color);
            draw_text((dim.width - width) / 2.f + offset, y - offset, (u8 *) text, &state->assets.menu_item_font, front_color);
        } else {
            draw_text((dim.width - width) / 2.f, y, (u8 *) text, &state->assets.menu_item_font, default_color);
        }
        
        y += (real32) state->assets.menu_item_font.line_height;
    }
}

internal void
draw_game_view(memory_puzzle_state *state) {
    //
    // Draw field
    //
    if (state->game_mode == GameMode_Playing) {
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
                    
                    case MemoryCard_Oval: {
                        draw_oval(&card, min, max);
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
                    case MemoryCard_DoubleSquare: {
                        draw_double_square(&card, min, max);
                    } break;
                    case MemoryCard_Circle: {
                        draw_circle(&card, min, max);
                    } break;
                    case MemoryCard_Diamond: {
                        draw_diamond(&card, min, max);
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
        
        //
        // Draw HUD
        //
        
        memory_puzzle_level level = state->current_level;
        
        v2i dim = state->world.dimensions;
        
        char buffer[256];
        // TODO: Platform specific sprintf()
        wsprintfA(buffer, "Remaining Tries: %d", level.max_tries - level.tries);
        
        draw_text(dim.width * 0.02f, dim.height * 0.05f, (u8 *) buffer, &state->assets.primary_font, make_color(0xffffffff));
    } else {
        draw_menu(state);
    }
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
        
        memory_puzzle_assets assets = {};
        assets.primary_font = load_font("./data/fonts/Inconsolata-Regular.ttf", 24.f);
        assets.menu_title_font = load_font("./data/fonts/Inconsolata-Bold.ttf", 48.f);
        assets.menu_item_font = load_font("./data/fonts/Inconsolata-Bold.ttf", 36.f);
        
        state->game_mode = GameMode_Playing;
        state->assets = assets;
        state->checking_cards_target = 1.f;
        
        state->world = init_world();
        init_level(state);
        
        memory->initialized = true;
    }
    
    state->world.dimensions = memory->window_dimensions;
    
    //
    // Update
    //
    
    if (state->game_mode == GameMode_Playing) {
        
        if (pressed(Button_Escape)) {
            state->game_mode = GameMode_Menu;
        } else {
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
                state->checking_cards_t += time_info.dt;
                
                if (state->checking_cards_t > state->checking_cards_target) {
                    state->checking_cards_t = .0f;
                    
                    memory_card *first = state->first_flipped;
                    memory_card *second = state->second_flipped;
                    if (flipped_cards_match(first, second)) {
                        state->flip_count++;
                    } else {
                        state->current_level.tries++;
                        if (state->current_level.tries >= state->current_level.max_tries) {
                            state->game_mode = GameMode_GameOver;
                        }
                        
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
                            state->checking_cards_t = time_info.dt;
                        }
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
                    memory_puzzle_game_restart(state);
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
    
    // Check for winning condition then restart.
    if (state->flip_count >= array_count(state->world.field) * array_count(state->world.field[0])) {
        memory_puzzle_game_restart(state);
    }
    
    draw_game_view(state);
}