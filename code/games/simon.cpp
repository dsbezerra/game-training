internal Simon_Button_Color
random_button() {
    Simon_Button_Color result = SimonButton_None;
    
    while (result == SimonButton_None) {
        if (random_choice(1)) {
            result = SimonButton_Yellow;
        }
        if (random_choice(2)) {
            result = SimonButton_Blue;
        }
        if (random_choice(3)) {
            result = SimonButton_Red;
        }
        if (random_choice(4)) {
            result = SimonButton_Green;
        }
    }
    
    return result;
}

internal void
init_pattern(Simon_State *state) {
    state->mode = SimonMode_DisplayPattern;
    for (int i = 0; i < array_count(state->pattern); ++i) {
        state->pattern[i] = random_button();
    }
    state->displaying_count = 4;
    state->displaying_index = 0;
    state->player_index = 0;
    state->hovering_button = SimonButton_None;
    
    state->flashing_t = .0f;
    state->flashing_t_target = 1.f;
    state->flashing_dt = 4.f;
}

internal void
advance_pattern(Simon_State *state) {
    state->displaying_count++;
    
    state->displaying_index = 0;
    state->player_index = 0;
    state->hovering_button = SimonButton_None;
    
    state->flashing_t = .0f;
    state->flashing_t_target = 1.f;
    
    state->mode = SimonMode_DisplayPattern;
}

internal b32
is_inside_button(Vector2 offset, real32 pad, real32 button_size, Vector2i mouse_p, Simon_Button_Color button) {
    b32 result = false;
    
    int x = 0;
    int y = 0;
    
    if (button == SimonButton_Blue) {
        x = 1;
        y = 0;
    } else if (button == SimonButton_Red) {
        x = 0;
        y = 1;
    } else if (button == SimonButton_Green) {
        x = 1;
        y = 1;
    }
    
    Vector2 min = make_vector2(button_size * x + pad, button_size * y + pad);
    Vector2 max = make_vector2(button_size * x + button_size, button_size * y + button_size);
    
    min = min + offset;
    max = max + offset;
    
    result = mouse_p.x >= min.x && mouse_p.x <= max.x && mouse_p.y >= min.y && mouse_p.y <= max.y;
    
    return result;
}

internal void
update_hovering_button(Simon_State *state) {
    Simon_Button_Color result = SimonButton_None;
    
    Vector2i dim = state->dimensions;
    
    real32 pad;
    if (dim.width > dim.height) {
        pad = dim.height * .05f;
    } else {
        pad = dim.width * .05f;
    }
    
    real32 max_height = dim.height * .8f;
    real32 button_size = (max_height - 2 * pad) / 2;
    
    Vector2 offset_to_center = make_vector2((dim.width  - button_size * 2) * .5f,
                                            (dim.height - button_size * 2) * .5f);
    
    Vector2i mouse_p = state->mouse_position;
    
    if (is_inside_button(offset_to_center, pad, button_size, mouse_p, SimonButton_Yellow)) {
        result = SimonButton_Yellow;
    } else if (is_inside_button(offset_to_center, pad, button_size, mouse_p, SimonButton_Blue)) {
        result = SimonButton_Blue;
    } else if (is_inside_button(offset_to_center, pad, button_size, mouse_p, SimonButton_Red)) {
        result = SimonButton_Red;
    } else if (is_inside_button(offset_to_center, pad, button_size, mouse_p, SimonButton_Green)) {
        result = SimonButton_Green;
    }
    
    state->hovering_button = result;
}

internal void
draw_game_view(Simon_State *state) {
    
    game_frame_begin(state->dimensions.width, state->dimensions.height);
    
    if (state->game_mode == GameMode_Playing) {
        //
        // Draw buttons
        //
        
        Vector2i dim = state->dimensions;
        
        real32 pad;
        if (dim.width > dim.height) {
            pad = dim.height * .05f;
        } else {
            pad = dim.width * .05f;
        }
        
        real32 max_height = dim.height * .8f;
        real32 button_size = (max_height - 2 * pad) / 2;
        
        Vector2 offset_to_center = make_vector2((dim.width  - button_size * 2) * .5f,
                                                (dim.height - button_size * 2) * .5f);
        
        u8 x = 0;
        u8 y = 0;
        
        immediate_begin();
        
        Vector4 yellow = make_color(0x33ffff00);
        Vector4 red = make_color(0x33ff0000);
        Vector4 blue = make_color(0x330000ff);
        Vector4 green = make_color(0x3300ff00);
        
        Simon_Button_Color button = SimonButton_None;
        if (state->mode == SimonMode_Playing) {
            button = state->hovering_button;
        } else if (state->mode == SimonMode_DisplayPattern) {
            button = state->pattern[state->displaying_index];
        }
        if (button == SimonButton_Yellow) {
            Vector4 opaque_yellow = make_color(0xffffff00);
            yellow = lerp_color(yellow, state->flashing_t, opaque_yellow);
        }
        if (button == SimonButton_Red) {
            Vector4 opaque_red = make_color(0xffff0000);
            red = lerp_color(red, state->flashing_t, opaque_red);
        }
        if (button == SimonButton_Blue) {
            Vector4 opaque_blue = make_color(0xff0000ff);
            blue = lerp_color(blue, state->flashing_t, opaque_blue);
        }
        if (button == SimonButton_Green) {
            Vector4 opaque_green = make_color(0xff00ff00);
            green = lerp_color(green, state->flashing_t, opaque_green);
        }
        
        // Yellow
        {
            Vector2 min = make_vector2(button_size * x + pad, button_size * y + pad);
            Vector2 max = make_vector2(button_size * x + button_size, button_size * y + button_size);
            min = min + offset_to_center;
            max = max + offset_to_center;
            immediate_quad(min, max, yellow);
        }
        y = 1;
        // Red
        {
            Vector2 min = make_vector2(button_size * x + pad, button_size * y + pad);
            Vector2 max = make_vector2(button_size * x + button_size, button_size * y + button_size);
            min = min + offset_to_center;
            max = max + offset_to_center;
            immediate_quad(min, max, red);
        }
        x = 1;
        y = 0;
        // Blue
        {
            Vector2 min = make_vector2(button_size * x + pad, button_size * y + pad);
            Vector2 max = make_vector2(button_size * x + button_size, button_size * y + button_size);
            min = min + offset_to_center;
            max = max + offset_to_center;
            immediate_quad(min, max, blue);
        }
        y = 1;
        // Green
        {
            Vector2 min = make_vector2(button_size * x + pad, button_size * y + pad);
            Vector2 max = make_vector2(button_size * x + button_size, button_size * y + button_size);
            min = min + offset_to_center;
            max = max + offset_to_center;
            immediate_quad(min, max, green);
        }
        
        immediate_flush();
    } else {
        draw_menu(SIMON_TITLE, state->memory);
    }
    
} 

internal void
simon_menu_art(App_State *state, Vector2 min, Vector2 max) {
    immediate_begin();
    immediate_textured_quad(min, max, state->menu_art.simon);
    immediate_flush();
}

internal void
simon_game_restart(Simon_State *state) {
    state->game_mode = GameMode_Playing;
    state->memory->game_mode = GameMode_Playing;
    state->memory->asked_to_quit = false;
    init_pattern(state);
}

internal void
simon_game_update_and_render(Game_Memory *memory, Game_Input *input) {
    
    Simon_State *state = (Simon_State *) memory->permanent_storage;
    if (!memory->initialized) {
        assert(memory->permanent_storage_size == 0);
        assert(!memory->permanent_storage);
        memory->initialized = true;
        
        state = (Simon_State *) game_alloc(memory, megabytes(12));
        state->memory = memory;
        
        init_pattern(state);
        platform_show_cursor(true);
    }
    
    state->dimensions = memory->window_dimensions;
    
    //
    // Update
    //
    Vector2i new_mouse_position;
    platform_get_cursor_position(&new_mouse_position);
    state->mouse_position = new_mouse_position;
    
    Simulate_Game sim = game_simulate(memory, input, state->game_mode);
    switch (sim.operation) {
        case SimulateGameOp_Update: {
            if (state->mode == SimonMode_Playing) {
                update_hovering_button(state);
                if (released(Button_Mouse1)) {
                    if (state->hovering_button != SimonButton_None) {
                        if (state->pattern[state->player_index++] != state->hovering_button) {
                            init_pattern(state);
                        }
                    }
                }
                
                if (state->player_index >= state->displaying_count) {
                    if (state->displaying_count < array_count(state->pattern) - 1) {
                        advance_pattern(state);
                    } else {
                        state->game_mode = GameMode_Menu;
                    }
                }
                
            } else if (state->mode == SimonMode_DisplayPattern) {
                if (state->flashing_t < state->flashing_t_target) {
                    state->flashing_t += core.time_info.dt * state->flashing_dt;
                    if (state->flashing_t > state->flashing_t_target) {
                        state->flashing_t_target = .0f;
                    }
                } else if (state->flashing_t > state->flashing_t_target) {
                    state->flashing_t -= core.time_info.dt * state->flashing_dt;
                    if (state->flashing_t < state->flashing_t_target) {
                        state->flashing_t_target = 1.f;
                        state->displaying_index++;
                        if (state->displaying_index >= state->displaying_count) {
                            state->flashing_t = 1.f;
                            state->mode = SimonMode_Playing;
                        }
                    }
                }
            }
        } break;
        
        case SimulateGameOp_Restart: {
            simon_game_restart(state);
        } break;
        
        default: break;
    }
    
    //
    // Draw
    //
    
    draw_game_view(state);
}
