internal void
init_game(sokoban_state *state) {
    state->game_mode = GameMode_Playing;
    init_camera(&state->cam);
    set_camera_mode(&state->cam, CameraMode_Free);
    
    state->cam.position.z = 3.f;
}

internal void
update_game(sokoban_state *state, game_input *input) {
    // TODO
    update_camera(&state->cam, input);
}

internal void
draw_game_view(sokoban_state *state) {
    if (state->game_mode == GameMode_Playing) {
        // TODO
    } else {
        draw_menu(SOKOBAN_TITLE, state->dimensions, state->game_mode, state->menu_selected_item, state->quit_was_selected);
    }
}

internal void
sokoban_menu_art(app_state *state, v2 min, v2 max) {
    v4 background = make_color(0xFFFF00FF);
    immediate_begin();
    immediate_quad(min, max, background);
    immediate_flush();
}

internal void
sokoban_game_restart(sokoban_state *state) {
    init_game(state);
}

internal void
sokoban_game_update_and_render(game_memory *memory, game_input *input) {
    sokoban_state *state = (sokoban_state *) memory->permanent_storage;
    if (!memory->initialized) {
        assert(memory->permanent_storage_size == 0);
        assert(!memory->permanent_storage);
        memory->initialized = true;
        
        state = (sokoban_state *) game_alloc(memory, megabytes(12));
        state->assets.box = load_texture("./data/textures/sokoban/container.jpg");
        
        state->cam.position = make_v3(.0f, 0.f, -3.f);
        
        init_game(state);
        
        // TODO(diego): Better way to do this
        input->mouse.sensitivity = 0.05f;
    }
    state->dimensions = memory->window_dimensions;
    
    // NOTE(diego): Lock mouse to center of screen and use new position
    // to calculate velocity and move our player with it.
    {
        v2i new_mouse_position;
        platform_get_cursor_position(&new_mouse_position);
        
        input->mouse.velocity = sub(new_mouse_position, input->mouse.position);
        input->mouse.velocity.y *= -1; // NOTE(diego): Make mouse down move pitch down
        
        if (input->mouse.lock) {
            platform_set_cursor_position(memory->window_center);
            input->mouse.position = memory->window_center;
        } else {
            input->mouse.position = new_mouse_position;
        }
        platform_show_cursor(false);
    }
    
    //
    // Update
    //
    if (state->game_mode == GameMode_Playing) {
        if (pressed(Button_Escape)) {
            state->game_mode = GameMode_Menu;
        } else {
            update_game(state, input);
        }
    } else if (state->game_mode == GameMode_Menu ||
               state->game_mode == GameMode_GameOver) {
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
                    sokoban_game_restart(state);
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
    
#define TEST_3D 1
    
#if TEST_3D
    
    
    render_3d(state->dimensions.width, state->dimensions.height, state->cam.fov);
    
    immediate_begin();
    
    v4 color = make_color(0xffffffff);
    
    v3 positions[] = {
        make_v3(0.f, 0.f, 0.f),
        make_v3( 2.0f,  5.0f, -15.0f),
        make_v3(-1.5f, -2.2f, -2.5f),
        make_v3(-3.8f, -2.0f, -12.3f),
        make_v3 (2.4f, -0.4f, -3.5f),
        make_v3(-1.7f,  3.0f, -7.5f),
        make_v3( 1.3f, -2.0f, -2.5f),
        make_v3( 1.5f,  2.0f, -2.5f),
        make_v3( 1.5f,  0.2f, -1.5f),
        make_v3(-1.3f,  1.0f, -1.5f)
    };
    
    view_matrix = get_view_matrix(&state->cam);
    
    open_gl->glUniformMatrix4fv(global_basic_3d_shader.view_loc, 1, GL_FALSE, view_matrix.e);
    
    open_gl->glUniform1i(immediate->current_shader.texture_loc, 0);
    glBindTexture(GL_TEXTURE_2D, state->assets.box);
    open_gl->glActiveTexture(GL_TEXTURE0);
    
    for (int i = 0; i < array_count(positions); ++i) {
        immediate_begin();
        
        real32 angle = 20.f * i;
        
        mat4 model_matrix = translate(positions[i]);
        model_matrix = model_matrix * rotation(angle, make_v3(1.f, .3f, .5f));
        
        open_gl->glUniformMatrix4fv(global_basic_3d_shader.model_loc, 1, GL_FALSE, model_matrix.e);
        
        immediate_vertex(make_v3( .5f,  .5f,  .5f), color, make_v2(1.0f, .0f));
        immediate_vertex(make_v3( .5f,  .5f, -.5f), color, make_v2(1.f, 1.0f));
        immediate_vertex(make_v3( .5f, -.5f, -.5f), color, make_v2(0.f, 1.f));
        immediate_vertex(make_v3( .5f, -.5f, -.5f), color, make_v2(0.f, 1.f));
        immediate_vertex(make_v3( .5f, -.5f,  .5f), color, make_v2(.0f, 0.f));
        immediate_vertex(make_v3( .5f,  .5f,  .5f), color, make_v2(1.0f, .0f));
        
        immediate_vertex(make_v3(-.5f, -.5f, -.5f), color, make_v2(.0f, 1.0f));
        immediate_vertex(make_v3( .5f, -.5f, -.5f), color, make_v2(1.f, 1.0f));
        immediate_vertex(make_v3( .5f, -.5f,  .5f), color, make_v2(1.f, 0.f));
        immediate_vertex(make_v3( .5f, -.5f,  .5f), color, make_v2(1.f, 0.f));
        immediate_vertex(make_v3(-.5f, -.5f,  .5f), color, make_v2(.0f, 0.f));
        immediate_vertex(make_v3(-.5f, -.5f, -.5f), color, make_v2(.0f, 1.0f));
        
        immediate_vertex(make_v3(-.5f,  .5f, -.5f), color, make_v2(.0f, 1.0f));
        immediate_vertex(make_v3( .5f,  .5f, -.5f), color, make_v2(1.f, 1.0f));
        immediate_vertex(make_v3( .5f,  .5f,  .5f), color, make_v2(1.f, 0.f));
        immediate_vertex(make_v3( .5f,  .5f,  .5f), color, make_v2(1.f, 0.f));
        immediate_vertex(make_v3(-.5f,  .5f,  .5f), color, make_v2(.0f, 0.f));
        immediate_vertex(make_v3(-.5f,  .5f, -.5f), color, make_v2(.0f, 1.0f));
        
        immediate_vertex(make_v3(-.5f, -.5f, -.5f), color, make_v2(.0f, .0f));
        immediate_vertex(make_v3( .5f, -.5f, -.5f), color, make_v2(1.f, .0f));
        immediate_vertex(make_v3( .5f,  .5f, -.5f), color, make_v2(1.f, 1.f));
        immediate_vertex(make_v3( .5f,  .5f, -.5f), color, make_v2(1.f, 1.f));
        immediate_vertex(make_v3(-.5f,  .5f, -.5f), color, make_v2(.0f, 1.f));
        immediate_vertex(make_v3(-.5f, -.5f, -.5f), color, make_v2(.0f, .0f));
        
        immediate_vertex(make_v3(-.5f, -.5f,  .5f), color, make_v2(.0f, .0f));
        immediate_vertex(make_v3( .5f, -.5f,  .5f), color, make_v2(1.f, .0f));
        immediate_vertex(make_v3( .5f,  .5f,  .5f), color, make_v2(1.f, 1.f));
        immediate_vertex(make_v3( .5f,  .5f,  .5f), color, make_v2(1.f, 1.f));
        immediate_vertex(make_v3(-.5f,  .5f,  .5f), color, make_v2(.0f, 1.f));
        immediate_vertex(make_v3(-.5f, -.5f,  .5f), color, make_v2(.0f, .0f));
        
        immediate_vertex(make_v3(-.5f,  .5f,  .5f), color, make_v2(1.0f, .0f));
        immediate_vertex(make_v3(-.5f,  .5f, -.5f), color, make_v2(1.f, 1.0f));
        immediate_vertex(make_v3(-.5f, -.5f, -.5f), color, make_v2(0.f, 1.f));
        immediate_vertex(make_v3(-.5f, -.5f, -.5f), color, make_v2(0.f, 1.f));
        immediate_vertex(make_v3(-.5f, -.5f,  .5f), color, make_v2(.0f, 0.f));
        immediate_vertex(make_v3(-.5f,  .5f,  .5f), color, make_v2(1.0f, .0f));
        immediate_flush();
    }
    
#endif
    
    draw_game_view(state);
}
