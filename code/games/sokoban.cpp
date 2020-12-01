internal void
init_game(sokoban_state *state) {
    state->game_mode = GameMode_Playing;
    init_camera(&state->cam);
    set_camera_mode(&state->cam, CameraMode_Free);
    
    state->cam.position.z = 3.f;
    
    
    v4 white = make_color(0xffffffff);
    v4 black = make_color(0xff000000);
    
    for (int tile_y = 0; tile_y < 16; tile_y++) {
        for (int tile_x = 0; tile_x < 16; tile_x++) {
            
            sokoban_tile tile = {};
            
            v3 pos = make_v3(tile_x * .4f, .0f, tile_y * 0.4f);
            tile.position.x = pos.x;
            tile.position.y = pos.y;
            tile.position.z = pos.z;
            
            if ((tile_x + tile_y) % 2 == 0) {
                tile.color = white;
            } else {
                tile.color = black;
            }
            
            state->tiles[tile_x][tile_y] = tile;
        }
    }
    
    
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
        
        
#define TEST_3D 1
        
#if TEST_3D
        
        
        render_3d(state->dimensions.width, state->dimensions.height, state->cam.fov);
        
        immediate_begin();
        
        v4 color = make_color(0xffaaaaaa);
        
        view_matrix = get_view_matrix(&state->cam);
        
        open_gl->glUniformMatrix4fv(global_basic_3d_shader.view_loc, 1, GL_FALSE, view_matrix.e);
        
        open_gl->glUniform1i(immediate->current_shader.texture_loc, 0);
        glBindTexture(GL_TEXTURE_2D, state->assets.none);
        open_gl->glActiveTexture(GL_TEXTURE0);
        
        sokoban_tile *tiles_ptr = (sokoban_tile *) state->tiles;
        int len = 16*16;
        for (sokoban_tile *tile = tiles_ptr; tile != tiles_ptr + len; tile++) {
            immediate_begin();
            
            mat4 model_matrix = translate(tile->position);
            open_gl->glUniformMatrix4fv(global_basic_3d_shader.model_loc, 1, GL_FALSE, model_matrix.e);
            
            immediate_vertex(make_v3( .2f,  .2f,  .2f), tile->color, make_v2(1.0f, .0f));
            immediate_vertex(make_v3( .2f,  .2f, -.2f), tile->color, make_v2(1.f, 1.0f));
            immediate_vertex(make_v3( .2f, -.2f, -.2f), tile->color, make_v2(0.f, 1.f));
            immediate_vertex(make_v3( .2f, -.2f, -.2f), tile->color, make_v2(0.f, 1.f));
            immediate_vertex(make_v3( .2f, -.2f,  .2f), tile->color, make_v2(.0f, 0.f));
            immediate_vertex(make_v3( .2f,  .2f,  .2f), tile->color, make_v2(1.0f, .0f));
            
            immediate_vertex(make_v3(-.2f, -.2f, -.2f), tile->color, make_v2(.0f, 1.0f));
            immediate_vertex(make_v3( .2f, -.2f, -.2f), tile->color, make_v2(1.f, 1.0f));
            immediate_vertex(make_v3( .2f, -.2f,  .2f), tile->color, make_v2(1.f, 0.f));
            immediate_vertex(make_v3( .2f, -.2f,  .2f), tile->color, make_v2(1.f, 0.f));
            immediate_vertex(make_v3(-.2f, -.2f,  .2f), tile->color, make_v2(.0f, 0.f));
            immediate_vertex(make_v3(-.2f, -.2f, -.2f), tile->color, make_v2(.0f, 1.0f));
            
            immediate_vertex(make_v3(-.2f,  .2f, -.2f), tile->color, make_v2(.0f, 1.0f));
            immediate_vertex(make_v3( .2f,  .2f, -.2f), tile->color, make_v2(1.f, 1.0f));
            immediate_vertex(make_v3( .2f,  .2f,  .2f), tile->color, make_v2(1.f, 0.f));
            immediate_vertex(make_v3( .2f,  .2f,  .2f), tile->color, make_v2(1.f, 0.f));
            immediate_vertex(make_v3(-.2f,  .2f,  .2f), tile->color, make_v2(.0f, 0.f));
            immediate_vertex(make_v3(-.2f,  .2f, -.2f), tile->color, make_v2(.0f, 1.0f));
            
            immediate_vertex(make_v3(-.2f, -.2f, -.2f), tile->color, make_v2(.0f, .0f));
            immediate_vertex(make_v3( .2f, -.2f, -.2f), tile->color, make_v2(1.f, .0f));
            immediate_vertex(make_v3( .2f,  .2f, -.2f), tile->color, make_v2(1.f, 1.f));
            immediate_vertex(make_v3( .2f,  .2f, -.2f), tile->color, make_v2(1.f, 1.f));
            immediate_vertex(make_v3(-.2f,  .2f, -.2f), tile->color, make_v2(.0f, 1.f));
            immediate_vertex(make_v3(-.2f, -.2f, -.2f), tile->color, make_v2(.0f, .0f));
            
            immediate_vertex(make_v3(-.2f, -.2f,  .2f), tile->color, make_v2(.0f, .0f));
            immediate_vertex(make_v3( .2f, -.2f,  .2f), tile->color, make_v2(1.f, .0f));
            immediate_vertex(make_v3( .2f,  .2f,  .2f), tile->color, make_v2(1.f, 1.f));
            immediate_vertex(make_v3( .2f,  .2f,  .2f), tile->color, make_v2(1.f, 1.f));
            immediate_vertex(make_v3(-.2f,  .2f,  .2f), tile->color, make_v2(.0f, 1.f));
            immediate_vertex(make_v3(-.2f, -.2f,  .2f), tile->color, make_v2(.0f, .0f));
            
            immediate_vertex(make_v3(-.2f,  .2f,  .2f), tile->color, make_v2(1.0f, .0f));
            immediate_vertex(make_v3(-.2f,  .2f, -.2f), tile->color, make_v2(1.f, 1.0f));
            immediate_vertex(make_v3(-.2f, -.2f, -.2f), tile->color, make_v2(0.f, 1.f));
            immediate_vertex(make_v3(-.2f, -.2f, -.2f), tile->color, make_v2(0.f, 1.f));
            immediate_vertex(make_v3(-.2f, -.2f,  .2f), tile->color, make_v2(.0f, 0.f));
            immediate_vertex(make_v3(-.2f,  .2f,  .2f), tile->color, make_v2(1.0f, .0f));
            immediate_flush();
        }
        
#endif
        
        
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
        state->assets.none = load_texture("./data/textures/sokoban/none.jpg");
        
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
    draw_game_view(state);
}
