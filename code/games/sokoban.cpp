internal void
init_game(sokoban_state *state) {
    // TODO
}

internal void
update_game(sokoban_state *state, game_input *input) {
    // TODO
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
    
    set_shader(global_basic_3d_shader);
    
    render_3d(state->dimensions.width, state->dimensions.height);
    
#define TEST_3D 1
    
#if TEST_3D
    
    immediate_begin();
    
    v4 color = make_color(0xffffffff);
    
    view_matrix = view_matrix * translate(make_v3(.0f, 0.f, -3.f));
    
    mat4 model_matrix = identity() * translate(make_v3(0.f, 0.f, 0.f));
    
    real32 t = cosf(time_info.current_time * 2.f);
    t *= t;
    
    
    model_matrix = model_matrix * rotation(time_info.current_time/250.f * 50.f, make_v3(1.f, .3f, .5f)) * scale(make_v3(t, t, t));
    
    open_gl->glUniformMatrix4fv(global_basic_3d_shader.model_loc, 1, GL_FALSE, model_matrix.e);
    refresh_shader_transform();
    
    open_gl->glUniform1i(immediate->current_shader.texture_loc, 0);
    glBindTexture(GL_TEXTURE_2D, state->assets.box);
    open_gl->glActiveTexture(GL_TEXTURE0);
    
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
    
#endif
    
    draw_game_view(state);
}
