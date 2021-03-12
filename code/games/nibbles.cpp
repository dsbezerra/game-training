internal Nibbles_Entity
make_entity(Nibbles_Entity_Kind kind, u8 x, u8 y) {
    assert(kind < NibblesEntity_Count);
    
    Nibbles_Entity result = {};
    
    result.kind = kind;
    result.x = x;
    result.y = y;
    
    return result;
}

internal void
init_board(Nibbles_State *state) {
    Nibbles_Entity apple = make_entity(NibblesEntity_Apple, 14, NIBBLES_WORLD_Y_COUNT / 2);
    Nibbles_Entity snake_head = make_entity(NibblesEntity_Snake, 6, NIBBLES_WORLD_Y_COUNT / 2);
    Nibbles_Entity snake_tail = make_entity(NibblesEntity_Snake, 5, NIBBLES_WORLD_Y_COUNT / 2);
    
    // Set entities
    u8 snake_index = 0;
    state->snake[snake_index++] = snake_head;
    state->snake[snake_index++] = snake_tail;
    state->snake_length = snake_index;
    
    state->apple = apple;
}

internal void
init_game(Nibbles_State *state) {
    state->game_mode = GameMode_Playing;
    state->memory->game_mode = GameMode_Playing;
    state->step_t = .0f;
    state->step_t_target = 1.f;
    state->step_dt = 12.f;
    
    state->direction = NibblesSnakeDirection_Right;
    
    init_board(state);
}

internal b32
is_occupied(Nibbles_State *state, u8 x, u8 y) {
    b32 result = false;
    
    for (int snake_index = 1; snake_index < state->snake_length; ++snake_index) {
        if (state->snake[snake_index].x == x && state->snake[snake_index].y == y) {
            result = true;
            break;
        }
    }
    
    return result;
}

internal void
advance_snake(Nibbles_State *state) {
    
    Nibbles_Entity *head = &state->snake[0];
    
    s8 new_head_x = head->x;
    s8 new_head_y = head->y;
    
    switch (state->direction) {
        case NibblesSnakeDirection_Up: {
            new_head_y -= 1;
        } break;
        case NibblesSnakeDirection_Down: {
            new_head_y += 1;
        } break;
        case NibblesSnakeDirection_Left: {
            new_head_x -= 1;
        } break;
        case NibblesSnakeDirection_Right: {
            new_head_x += 1;
        } break;
        default: {
            assert(!"Should not happen!");
        } break;
    }
    
    // Store old as previous so we can make the rest of the snake parts
    // follow the head.
    u8 last_part_x = head->x;
    u8 last_part_y = head->y;
    
    head->x = new_head_x;
    head->y = new_head_y;
    
    // Loop through all snake parts and update each one to previous part position
    for (int snake_index = 1; snake_index < state->snake_length; ++snake_index) {
        Nibbles_Entity *part = &state->snake[snake_index];
        u8 part_x = part->x;
        u8 part_y = part->y;
        
        part->x = last_part_x;
        part->y = last_part_y;
        
        last_part_x = part_x;
        last_part_y = part_y;
    }
    
    // Check if new position is valid
    if (is_occupied(state, head->x, head->y) || 
        head->x > NIBBLES_WORLD_X_COUNT || head->x < 0 || 
        head->y > NIBBLES_WORLD_Y_COUNT || head->y < 0) {
        state->game_mode = GameMode_GameOver;
        state->memory->game_mode = GameMode_GameOver;
        return;
    }
    
    // Check if new position has an apple
    if (state->apple.kind != NibblesEntity_None) {
        if (head->x == state->apple.x && head->y == state->apple.y) {
            state->apple.kind = NibblesEntity_None;
            Nibbles_Entity *last = &state->snake[state->snake_length - 1];
            state->snake[state->snake_length++] = make_entity(NibblesEntity_Snake, last->x, last->y);
            state->last_eaten_apple_time = core.time_info.current_time;
        }
    }
}

internal void
draw_apple(Nibbles_State *state, Nibbles_Entity *apple) {
    Vector2i dim = state->dimensions;
    
    real32 pad = 0.05f;
    real32 width = dim.width - dim.width * pad;
    real32 height = dim.height - dim.height * pad;
    
    real32 apple_size = width / (real32) NIBBLES_WORLD_X_COUNT;
    if (width > height) {
        apple_size = height / (real32) NIBBLES_WORLD_Y_COUNT;
    }
    
    real32 start_x = apple_size * apple->x;
    real32 start_y = apple_size * apple->y;
    
    real32 apple_pad = apple_size * 0.75f;
    
    Vector4 color = make_color(0xffff0000);
    
    Vector2 min = make_vector2(start_x + apple_pad             , start_y + apple_pad);
    Vector2 max = make_vector2(start_x + apple_size - apple_pad, start_y + apple_size - apple_pad);
    
    immediate_quad(min, max, color);
}

internal void
draw_snake(Nibbles_State *state) {
    Vector2i dim = state->dimensions;
    
    real32 pad = 0.05f;
    real32 width = dim.width - dim.width * pad;
    real32 height = dim.height - dim.height * pad;
    
    real32 snake_size = width / (real32) NIBBLES_WORLD_X_COUNT;
    if (width > height) {
        snake_size = height / (real32) NIBBLES_WORLD_Y_COUNT;
    }
    
    Vector4 color = make_color(0xff00ff00);
    
    for (int snake_index = 0; snake_index < state->snake_length; ++snake_index) {
        Nibbles_Entity *snake = &state->snake[snake_index];
        Vector2 min = make_vector2(snake_size * snake->x, snake_size * snake->y);
        Vector2 max = make_vector2(snake_size * snake->x + snake_size, snake_size * snake->y + snake_size);
        
        immediate_quad(min, max, color);
    }
}

internal void
draw_board(Nibbles_State *state) {
    immediate_begin();
    // NOTE(diego): Apple is None if not spawned
    if (state->apple.kind == NibblesEntity_Apple) {
        draw_apple(state, &state->apple);
    }
    draw_snake(state);
    immediate_flush();
}

internal void
draw_game_view(Nibbles_State *state) {
    game_frame_begin(state->dimensions.width, state->dimensions.height);
    if (state->game_mode == GameMode_Playing) {
        draw_board(state);
    } else {
        draw_menu(NIBBLES_TITLE, state->memory);
    }
} 

internal void
nibbles_menu_art(App_State *state, Vector2 min, Vector2 max) {
    immediate_begin();
    immediate_textured_quad(min, max, state->menu_art.nibbles);
    immediate_flush();
}

internal void
nibbles_game_restart(Nibbles_State *state) {
    //
    // Re-init
    //
    init_game(state);
}

internal void
nibbles_game_update_and_render(Game_Memory *memory, Game_Input *input) {
    
    Nibbles_State *state = (Nibbles_State *) memory->permanent_storage;
    if (!memory->initialized) {
        assert(memory->permanent_storage_size == 0);
        assert(!memory->permanent_storage);
        memory->initialized = true;
        
        state = (Nibbles_State *) game_alloc(memory, megabytes(12));
        state->memory = memory;
        
        init_game(state);
    }
    
    state->dimensions = memory->window_dimensions;
    
    Simulate_Game sim = game_simulate(memory, input, state->game_mode);
    switch (sim.operation) {
        case SimulateGameOp_Update: {
            Nibbles_Snake_Direction direction = state->direction;
            if (pressed(Button_Left) && state->direction != NibblesSnakeDirection_Right) {
                state->direction = NibblesSnakeDirection_Left;
            } else if (pressed(Button_Right) && state->direction != NibblesSnakeDirection_Left) {
                state->direction = NibblesSnakeDirection_Right;
            } else if (pressed(Button_Up) && state->direction != NibblesSnakeDirection_Down) {
                state->direction = NibblesSnakeDirection_Up;
            } else if (pressed(Button_Down) && state->direction != NibblesSnakeDirection_Up) {
                state->direction = NibblesSnakeDirection_Down;
            }
            if (state->apple.kind == NibblesEntity_None && (core.time_info.current_time - state->last_eaten_apple_time) > .5f) {
                while (1) {
                    u8 spawn_x = (u8) random_int_in_range(1, NIBBLES_WORLD_X_COUNT - 2);
                    u8 spawn_y = (u8) random_int_in_range(1, NIBBLES_WORLD_Y_COUNT - 2);
                    if (!is_occupied(state, spawn_x, spawn_y)) {
                        state->apple.kind = NibblesEntity_Apple;
                        state->apple.x = spawn_x;
                        state->apple.y = spawn_y;
                        break;
                    }
                }
            }
            if (state->step_t >= state->step_t_target || direction != state->direction) {
                state->step_t = .0f;
                advance_snake(state);
            }
            state->step_t += core.time_info.dt * state->step_dt;
        } break;
        
        case SimulateGameOp_Restart: {
            nibbles_game_restart(state);
        } break;
        
        default: break;
    }
    
    //
    // Draw
    //
    
    draw_game_view(state);
}
