internal nibbles_entity
make_entity(nibbles_entity_kind kind, u8 x, u8 y) {
    assert(kind < NibblesEntity_Count);
    
    nibbles_entity result = {};
    
    result.kind = kind;
    result.x = x;
    result.y = y;
    
    return result;
}

internal void
init_board(nibbles_state *state) {
    nibbles_entity apple = make_entity(NibblesEntity_Apple, 14, NIBBLES_WORLD_Y_COUNT / 2);
    nibbles_entity snake_head = make_entity(NibblesEntity_SnakeHead, 6, NIBBLES_WORLD_Y_COUNT / 2);
    nibbles_entity snake_tail = make_entity(NibblesEntity_SnakeTail, 5, NIBBLES_WORLD_Y_COUNT / 2);
    
    // Set entities
    u8 snake_index = 0;
    state->snake[snake_index++] = snake_head;
    state->snake[snake_index++] = snake_tail;
    state->snake_length = snake_index;
    
    state->apple = apple;
}

internal void
init_game(nibbles_state *state) {
    state->game_mode = GameMode_Playing;
    state->step_t = .0f;
    state->step_t_target = 1.f;
    state->step_dt = 12.f;
    
    state->moving_axis = NIBBLES_MOVING_AXIS_X;
    state->moving_direction = NIBBLES_MOVING_DIRECTION_POSITIVE;
    
    init_board(state);
}

internal void
advance_snake(nibbles_state *state) {
    
    u8 advance_x = 0;
    u8 advance_y = 0;
    
    if (state->moving_axis == NIBBLES_MOVING_AXIS_X) {
        advance_x = 1 * state->moving_direction;
    } else if (state->moving_axis == NIBBLES_MOVING_AXIS_Y) {
        advance_y = 1 * state->moving_direction;
    }
    
    nibbles_entity *head = &state->snake[0];
    
    u8 last_part_x = head->x;
    u8 last_part_y = head->y;
    
    head->x += advance_x;
    head->y += advance_y;
    
    // TODO(diego): Check if new head x and y are within the world bounds
    if (state->apple.kind != NibblesEntity_None) {
        if (head->x == state->apple.x && head->y == state->apple.y) {
            state->apple.kind = NibblesEntity_None;
            nibbles_entity *last = &state->snake[state->snake_length - 1];
            last->kind = NibblesEntity_SnakeBody;
            state->snake[state->snake_length++] = make_entity(NibblesEntity_SnakeBody, last->x, last->y);
            state->last_eaten_apple_time = time_info.current_time;
        }
    }
    
    // Skip head
    for (int snake_index = 1; snake_index < state->snake_length; ++snake_index) {
        nibbles_entity *part = &state->snake[snake_index];
        u8 part_x = part->x;
        u8 part_y = part->y;
        
        part->x = last_part_x;
        part->y = last_part_y;
        
        last_part_x = part_x;
        last_part_y = part_y;
    }
    
}

internal void
draw_apple(nibbles_state *state, nibbles_entity *apple) {
    v2i dim = state->dimensions;
    
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
    
    v4 color = make_color(0xffff0000);
    
    v2 min = make_v2(start_x + apple_pad             , start_y + apple_pad);
    v2 max = make_v2(start_x + apple_size - apple_pad, start_y + apple_size - apple_pad);
    
    immediate_quad(min, max, color, 1.f);
}

internal void
draw_snake(nibbles_state *state) {
    v2i dim = state->dimensions;
    
    real32 pad = 0.05f;
    real32 width = dim.width - dim.width * pad;
    real32 height = dim.height - dim.height * pad;
    
    real32 snake_size = width / (real32) NIBBLES_WORLD_X_COUNT;
    if (width > height) {
        snake_size = height / (real32) NIBBLES_WORLD_Y_COUNT;
    }
    
    v4 color = make_color(0xff00ff00);
    
    for (int snake_index = 0; snake_index < state->snake_length; ++snake_index) {
        nibbles_entity *snake = &state->snake[snake_index];
        v2 min = make_v2(snake_size * snake->x, snake_size * snake->y);
        v2 max = make_v2(snake_size * snake->x + snake_size, snake_size * snake->y + snake_size);
        
        immediate_quad(min, max, color, 1.f);
    }
}

internal void
draw_board(nibbles_state *state) {
    immediate_begin();
    // NOTE(diego): Apple is None if not spawned
    if (state->apple.kind == NibblesEntity_Apple) {
        draw_apple(state, &state->apple);
    }
    draw_snake(state);
    immediate_flush();
}

internal void
draw_game_view(nibbles_state *state) {
    if (state->game_mode == GameMode_Playing) {
        draw_board(state);
    } else {
        draw_menu(NIBBLES_TITLE, state->dimensions, state->game_mode, state->menu_selected_item, state->quit_was_selected);
    }
} 

internal void
nibbles_menu_art(v2 min, v2 max) {
    v4 background = make_color(0xff5a105f);
    immediate_begin();
    immediate_quad(min, max, background, 1.f);
    immediate_flush();
}

internal void
nibbles_game_restart(nibbles_state *state) {
    //
    // Re-init
    //
    init_game(state);
}

internal void
nibbles_game_update_and_render(game_memory *memory, game_input *input) {
    
    nibbles_state *state = (nibbles_state *) memory->permanent_storage;
    if (!memory->initialized) {
        assert(memory->permanent_storage_size == 0);
        assert(!memory->permanent_storage);
        memory->initialized = true;
        
        state = (nibbles_state *) game_alloc(memory, megabytes(12));
        
        init_game(state);
    }
    
    state->dimensions = memory->window_dimensions;
    
    //
    // Update
    //
    
    if (state->game_mode == GameMode_Playing) {
        if (pressed(Button_Escape)) {
            state->game_mode = GameMode_Menu;
        } else {
            b32 changed = false;
            u8 moving_axis = state->moving_axis;
            s8 moving_direction = state->moving_direction;
            
            if (moving_axis == NIBBLES_MOVING_AXIS_X) {
                if (pressed(Button_Up)) {
                    state->moving_axis = NIBBLES_MOVING_AXIS_Y;
                    state->moving_direction = NIBBLES_MOVING_DIRECTION_NEGATIVE;
                }
                if (pressed(Button_Down)) {
                    state->moving_axis = NIBBLES_MOVING_AXIS_Y;
                    state->moving_direction = NIBBLES_MOVING_DIRECTION_POSITIVE;
                }
            } else if (moving_axis == NIBBLES_MOVING_AXIS_Y) {
                if (pressed(Button_Right)) {
                    state->moving_axis = NIBBLES_MOVING_AXIS_X;
                    state->moving_direction = NIBBLES_MOVING_DIRECTION_POSITIVE;
                }
                if (pressed(Button_Left)) {
                    state->moving_axis = NIBBLES_MOVING_AXIS_X;
                    state->moving_direction = NIBBLES_MOVING_DIRECTION_NEGATIVE;
                }
            }
            
            if (state->apple.kind == NibblesEntity_None && (time_info.current_time - state->last_eaten_apple_time) > 2.f) {
                // TODO(diego): Check if spawn position is already occupied by snake
                state->apple = make_entity(NibblesEntity_Apple, (u8) random_int_in_range(0, NIBBLES_WORLD_X_COUNT), (u8) random_int_in_range(0, NIBBLES_WORLD_Y_COUNT));
            }
            
            if (state->step_t >= state->step_t_target) {
                state->step_t -= state->step_t_target;
                advance_snake(state);
            }
            
            state->step_t += time_info.dt * state->step_dt;
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
                    nibbles_game_restart(state);
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
    
    //
    // Draw
    //
    
    draw_game_view(state);
}
