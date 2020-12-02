internal simon_button_color
random_button() {
    simon_button_color result = SimonButton_None;
    
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
init_pattern(simon_state *state) {
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
advance_pattern(simon_state *state) {
    state->displaying_count++;
    
    state->displaying_index = 0;
    state->player_index = 0;
    state->hovering_button = SimonButton_None;
    
    state->flashing_t = .0f;
    state->flashing_t_target = 1.f;
    
    state->mode = SimonMode_DisplayPattern;
}

internal b32
is_inside_button(v2 offset, real32 pad, real32 button_size, v2i mouse_p, simon_button_color button) {
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
    
    v2 min = make_v2(button_size * x + pad, button_size * y + pad);
    v2 max = make_v2(button_size * x + button_size, button_size * y + button_size);
    
    min = min + offset;
    max = max + offset;
    
    result = mouse_p.x >= min.x && mouse_p.x <= max.x && mouse_p.y >= min.y && mouse_p.y <= max.y;
    
    return result;
}

internal void
update_hovering_button(simon_state *state) {
    simon_button_color result = SimonButton_None;
    
    v2i dim = state->dimensions;
    
    real32 pad;
    if (dim.width > dim.height) {
        pad = dim.height * .05f;
    } else {
        pad = dim.width * .05f;
    }
    
    real32 max_height = dim.height * .8f;
    real32 button_size = (max_height - 2 * pad) / 2;
    
    v2 offset_to_center = make_v2((dim.width  - button_size * 2) * .5f,
                                  (dim.height - button_size * 2) * .5f);
    
    v2i mouse_p = state->mouse_position;
    
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
draw_game_view(simon_state *state) {
    
    if (state->game_mode == GameMode_Playing) {
        //
        // Draw buttons
        //
        
        v2i dim = state->dimensions;
        
        real32 pad;
        if (dim.width > dim.height) {
            pad = dim.height * .05f;
        } else {
            pad = dim.width * .05f;
        }
        
        real32 max_height = dim.height * .8f;
        real32 button_size = (max_height - 2 * pad) / 2;
        
        v2 offset_to_center = make_v2((dim.width  - button_size * 2) * .5f,
                                      (dim.height - button_size * 2) * .5f);
        
        u8 x = 0;
        u8 y = 0;
        
        immediate_begin();
        
        v4 yellow = make_color(0x33ffff00);
        v4 red = make_color(0x33ff0000);
        v4 blue = make_color(0x330000ff);
        v4 green = make_color(0x3300ff00);
        
        simon_button_color button = SimonButton_None;
        if (state->mode == SimonMode_Playing) {
            button = state->hovering_button;
        } else if (state->mode == SimonMode_DisplayPattern) {
            button = state->pattern[state->displaying_index];
        }
        if (button == SimonButton_Yellow) {
            v4 opaque_yellow = make_color(0xffffff00);
            yellow = lerp_color(yellow, state->flashing_t, opaque_yellow);
        }
        if (button == SimonButton_Red) {
            v4 opaque_red = make_color(0xffff0000);
            red = lerp_color(red, state->flashing_t, opaque_red);
        }
        if (button == SimonButton_Blue) {
            v4 opaque_blue = make_color(0xff0000ff);
            blue = lerp_color(blue, state->flashing_t, opaque_blue);
        }
        if (button == SimonButton_Green) {
            v4 opaque_green = make_color(0xff00ff00);
            green = lerp_color(green, state->flashing_t, opaque_green);
        }
        
        // Yellow
        {
            v2 min = make_v2(button_size * x + pad, button_size * y + pad);
            v2 max = make_v2(button_size * x + button_size, button_size * y + button_size);
            min = min + offset_to_center;
            max = max + offset_to_center;
            immediate_quad(min, max, yellow);
        }
        y = 1;
        // Red
        {
            v2 min = make_v2(button_size * x + pad, button_size * y + pad);
            v2 max = make_v2(button_size * x + button_size, button_size * y + button_size);
            min = min + offset_to_center;
            max = max + offset_to_center;
            immediate_quad(min, max, red);
        }
        x = 1;
        y = 0;
        // Blue
        {
            v2 min = make_v2(button_size * x + pad, button_size * y + pad);
            v2 max = make_v2(button_size * x + button_size, button_size * y + button_size);
            min = min + offset_to_center;
            max = max + offset_to_center;
            immediate_quad(min, max, blue);
        }
        y = 1;
        // Green
        {
            v2 min = make_v2(button_size * x + pad, button_size * y + pad);
            v2 max = make_v2(button_size * x + button_size, button_size * y + button_size);
            min = min + offset_to_center;
            max = max + offset_to_center;
            immediate_quad(min, max, green);
        }
        
        immediate_flush();
    } else {
        draw_menu(SIMON_TITLE, state->dimensions, state->game_mode, state->menu_selected_item, state->quit_was_selected);
    }
    
} 

internal void
simon_menu_art(app_state *state, v2 min, v2 max) {
    immediate_begin();
    immediate_textured_quad(min, max, state->menu_art.simon);
    immediate_flush();
}

internal void
simon_game_restart(simon_state *state) {
    state->game_mode = GameMode_Playing;
    state->quit_was_selected = false;
    state->menu_selected_item = 0;
    
    //
    // Re-init
    //
    init_pattern(state);
}

internal void
simon_game_update_and_render(game_memory *memory, game_input *input) {
    
    simon_state *state = (simon_state *) memory->permanent_storage;
    if (!memory->initialized) {
        assert(memory->permanent_storage_size == 0);
        assert(!memory->permanent_storage);
        memory->initialized = true;
        
        state = (simon_state *) game_alloc(memory, megabytes(12));
        
        init_pattern(state);
    }
    
    state->dimensions = memory->window_dimensions;
    
    //
    // Update
    //
    v2i new_mouse_position;
    platform_get_cursor_position(&new_mouse_position);
    state->mouse_position = new_mouse_position;
    platform_show_cursor(true);
    
    if (state->game_mode == GameMode_Playing) {
        if (pressed(Button_Escape)) {
            state->game_mode = GameMode_Menu;
        } else {
            
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
                    state->flashing_t += time_info.dt * state->flashing_dt;
                    if (state->flashing_t > state->flashing_t_target) {
                        state->flashing_t_target = .0f;
                    }
                } else if (state->flashing_t > state->flashing_t_target) {
                    state->flashing_t -= time_info.dt * state->flashing_dt;
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
                    simon_game_restart(state);
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
