
internal void
init_level(Memory_Puzzle_State *state) {
    state->current_level = {};
    state->current_level.max_tries = 100;
}

internal void
memory_puzzle_game_restart(Memory_Puzzle_State *state) {
    state->Game_Mode = GameMode_Playing;
    state->flip_count = 0;
    state->quit_was_selected = false;
    state->menu_selected_item = 0;
    
    //
    // Re-init
    //
    state->world = init_world();
    
    init_level(state);
}

internal Vector4
get_random_card_color() {
    real32 t = random_real32_in_range(0.f, 2.f);
    Vector4 result = {};
    
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

internal Memory_Card
get_random_card() {
    Memory_Card result = {};
    
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

internal Memory_Puzzle_World
init_world() {
    Memory_Puzzle_World world = {}; // Ensure zero
    
    int x_count = array_count(world.field);
    int y_count = array_count(world.field[0]);
    int size = x_count * y_count;
    
    Memory_Card *cards = (Memory_Card *) world.field;
    for (Memory_Card *card = cards; card != cards + size;) {
        Memory_Card new_card = get_random_card();
        
        b32 contains = false;
        for (Memory_Card *c = cards; c != cards + size; c++) {
            if (c->kind == new_card.kind && c->color == new_card.color) {
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
        Memory_Card *first = cards + i;
        Memory_Card *second = cards + j;
        Memory_Card aux = *first;
        *first = *second;
        *second = aux;
    }
    
    return world;
}

internal Memory_Card *
get_current_selected_card(Memory_Puzzle_State *state) {
    int field_x = state->current_selected % array_count(state->world.field);
    int field_y = state->current_selected / array_count(state->world.field);
    
    return &state->world.field[field_x][field_y];
}

internal b32
flipped_cards_match(Memory_Card *first, Memory_Card *second) {
    if (!first || !second) return false;
    if (!first->flipped || !second->flipped) return false;
    
    return (first->kind == second->kind && first->color == second->color); 
}

internal void
draw_square(Memory_Card *card, Vector2 min, Vector2 max) {
    Vector2 square_size = (max - min) * 0.25f; 
    
    Vector2 square_min = min + square_size;
    Vector2 square_max = max - square_size;
    
    immediate_quad(square_min, square_max, card->color);
}

internal void
draw_oval(Memory_Card *card, Vector2 min, Vector2 max) {
    Vector2 radius = max - min;
    Vector2 center = make_vector2((min.x + max.x) / 2.f, (min.y + max.y) / 2.f);
    immediate_circle_filled(center, make_vector2(radius.x * 0.6f, radius.y * 0.3f), card->color);
}

internal void
draw_donut(Memory_Card *card, Vector2 min, Vector2 max) {
    Vector2 radius = max - min;
    Vector2 center = make_vector2((min.x + max.x) / 2.f, (min.y + max.y) / 2.f);
    immediate_circle(center, radius.x * 0.15f, radius.y * 0.4f, card->color);
}

internal void
draw_lines(Memory_Card *card, Vector2 min, Vector2 max) {
    
    real32 height = max.y - min.y;
    real32 line_height = 1.f;
    
    real32 line_count = 24.f;
    
    real32 advance_y = (height * line_height) / (line_count - 1);
    real32 y_cursor = min.y;
    
#if 0
    Vector2 default_uv = make_vector2(1.f, 1.f);
    real32 angle = -45.f;
#endif
    
    for (int y = 0; y < line_count - 1; y++) {
        
        // STUDY(diego): How to clip this properly to max min sizes...
#if 0
        Vector2 center = make_vector2((min.x + max.x) / 2.f, (min.y + max.y) / 2.f);
        
        Vector2 top_left = rotate_v2_around(make_vector2(min.x, y_cursor), center, angle);
        Vector2 top_right = rotate_v2_around(make_vector2(max.x, y_cursor), center, angle);
        Vector2 bottom_left = rotate_v2_around(make_vector2(min.x, y_cursor + line_height), center, angle);
        Vector2 bottom_right = rotate_v2_around(make_vector2(max.x, y_cursor + line_height), center, angle);
        
        immediate_vertex(top_left, card->color, default_uv, 1.f);
        immediate_vertex(bottom_left, card->color, default_uv, 1.f);
        immediate_vertex(bottom_right, card->color, default_uv, 1.f);
        
        immediate_vertex(top_right, card->color, default_uv, 1.f);
        immediate_vertex(bottom_right, card->color, default_uv, 1.f);
        immediate_vertex(top_left, card->color, default_uv, 1.f);
#endif
        
        Vector2 line_min = make_vector2(min.x, y_cursor);
        Vector2 line_max = make_vector2(max.x, y_cursor + line_height);
        immediate_quad(line_min, line_max, card->color);
        
        y_cursor += advance_y;
    }
}

internal void
draw_double_square(Memory_Card *card, Vector2 min, Vector2 max) {
    Vector2 size = (max - min) * 0.5f; 
    Vector2 first_min = min;
    Vector2 first_max = first_min + size;
    
    Vector2 second_min = first_max;
    Vector2 second_max = second_min + size;
    
    immediate_quad(first_min, first_max, card->color);
    immediate_quad(second_min, second_max, card->color);
}

internal void
draw_circle(Memory_Card *card, Vector2 min, Vector2 max) {
    Vector2 radius = (max - min) * 0.5f;
    Vector2 center = make_vector2((min.x + max.x) / 2.f, (min.y + max.y) / 2.f);
    immediate_circle_filled(center, radius, card->color);
}

internal void
draw_diamond(Memory_Card *card, Vector2 min, Vector2 max) {
    Vector2 half = make_vector2((min.x + max.x) / 2.f, (min.y + max.y) / 2.f);
    Vector2 default_uv = make_vector2(-1.f, -1.f);
    
    // First triangle
    real32 z_index = 1.f;
    immediate_vertex(make_vector3(half.x, min.y, z_index), card->color, default_uv);
    immediate_vertex(make_vector3(min.x, half.y, z_index), card->color, default_uv);
    immediate_vertex(make_vector3(half.x, max.y, z_index), card->color, default_uv);
    
    // Second triangle
    immediate_vertex(make_vector3(half.x, min.y, z_index), card->color, default_uv);
    immediate_vertex(make_vector3(half.x, max.y, z_index), card->color, default_uv);
    immediate_vertex(make_vector3(max.x, half.y, z_index), card->color, default_uv);
}

internal void
memory_puzzle_menu_art(App_State *state, Vector2 min, Vector2 max) {
    immediate_begin();
    immediate_textured_quad(min, max, state->menu_art.memory_puzzle);
    immediate_flush();
}

internal void
draw_game_view(Memory_Puzzle_State *state) {
    
    game_frame_begin(state->world.dimensions.width, state->world.dimensions.height);
    
    //
    // Draw field
    //
    if (state->Game_Mode == GameMode_Playing) {
        Memory_Puzzle_World world = state->world;
        
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
        
        Vector2 start = make_vector2((world.dimensions.width - card_size * x_count) / 2.f, (world.dimensions.height - card_size * y_count) / 2.f);
        immediate_begin();
        for (int field_y = 0; field_y < array_count(world.field[0]); ++field_y) {
            for (int field_x = 0; field_x < array_count(world.field); ++field_x) {
                Vector2 min = make_vector2(card_size * field_x + pad, card_size * field_y + pad);
                Vector2 max = make_vector2(card_size * field_x + card_size, field_y * card_size + card_size);
                
                min = min + start;
                max = max + start;
                
                Memory_Card card = world.field[field_x][field_y];
                if (!card.flipped) {
                    Vector4 white = make_color(0xffffffff);
                    immediate_quad(min, max, white);
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
            
            Vector2 min = make_vector2(card_size * field_x, card_size * field_y);
            Vector2 max = make_vector2(card_size * field_x + card_size, field_y * card_size + card_size);
            
            min = min + start;
            max = max + start;
            
            
            real32 stroke = pad * .5f;
            real32 half_stroke = stroke * .5f;
            
            min.x += stroke;
            max.x += stroke;
            
            min.y += stroke;
            max.y += stroke;
            
            Vector4 color = make_color(0xff0000ff);
            
            Vector2 left_min = make_vector2(min.x, min.y);
            Vector2 left_max = make_vector2(min.x + half_stroke, max.y);
            
            Vector2 right_min = make_vector2(max.x, min.y);
            Vector2 right_max = make_vector2(max.x - half_stroke, max.y);
            
            Vector2 top_min = make_vector2(min.x, min.y);
            Vector2 top_max = make_vector2(max.x, min.y + half_stroke);
            
            Vector2 bottom_min = make_vector2(min.x, max.y);
            Vector2 bottom_max = make_vector2(max.x, max.y - half_stroke);
            
            immediate_quad(left_min, left_max, color);
            immediate_quad(right_min, right_max, color);
            immediate_quad(top_min, top_max, color);
            immediate_quad(bottom_min, bottom_max, color);
        }
        
        immediate_flush();
        
        //
        // Draw HUD
        //
        
        Memory_Puzzle_Level level = state->current_level;
        
        Vector2i dim = state->world.dimensions;
        
        char buffer[256];
        // TODO: Platform specific sprintf()
        wsprintfA(buffer, "Remaining Tries: %d", level.max_tries - level.tries);
        
        draw_text(dim.width * 0.02f, dim.height * 0.05f, (u8 *) buffer, &state->assets.primary_font, make_color(0xffffffff));
    } else {
        draw_menu(MEMORY_PUZZLE_TITLE, state->world.dimensions, state->Game_Mode, state->menu_selected_item, state->quit_was_selected);
    }
}

internal void
memory_puzzle_game_update_and_render(Game_Memory *memory, Game_Input *input) {
    
    Memory_Puzzle_State *state = (Memory_Puzzle_State *) memory->permanent_storage;
    if (!memory->initialized) {
        assert(memory->permanent_storage_size == 0);
        assert(!memory->permanent_storage);
        
        state = (Memory_Puzzle_State *) game_alloc(memory, megabytes(12));
        
        Memory_Puzzle_Assets assets = {};
        assets.primary_font = load_font("./data/fonts/Inconsolata-Regular.ttf", 24.f);
        
        state->Game_Mode = GameMode_Playing;
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
    
    if (state->Game_Mode == GameMode_Playing) {
        
        if (pressed(Button_Escape)) {
            state->Game_Mode = GameMode_Menu;
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
                state->checking_cards_t += core.time_info.dt;
                
                if (state->checking_cards_t > state->checking_cards_target) {
                    state->checking_cards_t = .0f;
                    
                    Memory_Card *first = state->first_flipped;
                    Memory_Card *second = state->second_flipped;
                    if (flipped_cards_match(first, second)) {
                        state->flip_count++;
                    } else {
                        state->current_level.tries++;
                        if (state->current_level.tries >= state->current_level.max_tries) {
                            state->Game_Mode = GameMode_GameOver;
                        }
                        
                        first->flipped = false;
                        second->flipped = false;
                    }
                    state->first_flipped = 0;
                    state->second_flipped = 0;
                }
                
            } else {
                if (pressed(Button_Space)) {
                    Memory_Card *card = get_current_selected_card(state);
                    if (card && !card->flipped) {
                        card->flipped = !card->flipped;
                        if (!state->first_flipped) {
                            state->first_flipped = card;
                        } else if (!state->second_flipped) {
                            state->second_flipped = card;
                        }
                        
                        if (state->first_flipped && state->second_flipped) {
                            state->checking_cards_t = core.time_info.dt;
                        }
                    }
                }
            }
        }
    } else if (state->Game_Mode == GameMode_Menu || state->Game_Mode == GameMode_GameOver) {
        if (pressed(Button_Down)) {
            advance_menu_choice(&state->menu_selected_item, 1);
        }
        if (pressed(Button_Up)) {
            advance_menu_choice(&state->menu_selected_item, -1);
        }
        if (pressed(Button_Escape)) {
            if (state->Game_Mode == GameMode_GameOver) {
                memory->asked_to_quit = true;
            } else {
                state->Game_Mode = GameMode_Playing;
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