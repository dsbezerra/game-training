internal void
init_game(sokoban_state *state) {
    state->game_mode = GameMode_Playing;
    init_camera(&state->cam);
    set_camera_mode(&state->cam, CameraMode_Free);
    
    
    state->cam.position = make_v3(.0f, .0f, 3.0f);
    
    v4 white = make_color(0xffffffff);
    
    s32 stride = array_count(state->entities) / 16;
    
    s32 cube_rows = 5;
    
    s32 tile_x = 0;
    s32 tile_y = 0;
    
    // Light goes here
    
    // First 25 entities are tiles
    for (int i = 1; i < 2; i++) {
        
        sokoban_entity *entity = &state->entities[i];
        
        entity->kind = SokobanEntityKind_Tile;
        
        v3 p = make_v3(tile_x * .5f, .0f, tile_y * 0.5f);
        entity->position.x = p.x;
        entity->position.y = p.y;
        entity->position.z = p.z;
        
        entity->color = make_v4(1.0f, 1.0f, 1.0f, 1.f);
        if (tile_x >= cube_rows) {
            tile_x = 0;
            tile_y++;
        }
        
        tile_x++;
    }
    
    sokoban_entity *light = &state->entities[0];
    sokoban_entity cube = state->entities[8];
    
    light->kind = SokobanEntityKind_Light;
    light->position = make_v3(1.2f, 1.0f, 12.0f);
    light->color = make_color(0xffffffff);
    
}

internal void
update_game(sokoban_state *state, game_input *input) {
    // TODO
    update_camera(&state->cam, input);
    
    
    sokoban_entity *light = &state->entities[0];
    
    if (is_down(Button_W)) {
        light->position.y += 20.f * time_info.dt;
    }
    if (is_down(Button_S)) {
        light->position.y -= 20.f * time_info.dt;
    }
    
    if (is_down(Button_A)) {
        light->position.x += 20.f * time_info.dt;
    }
    if (is_down(Button_D)) {
        light->position.x -= 20.f * time_info.dt;
    }
}

internal void
draw_game_view(sokoban_state *state) {
    if (state->game_mode == GameMode_Playing) {
        // TODO
#define TEST_3D 1
#if TEST_3D
        
        render_3d(state->dimensions.width, state->dimensions.height, state->cam.fov);
        
        v4 white = make_color(0xffffffff);
        view_matrix = get_view_matrix(&state->cam);
        
        open_gl->glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, state->assets.container);
        
        open_gl->glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, state->assets.container_specular);
        
        sokoban_entity *light = &state->entities[0];
        
#if 0
        v4 light_color = {};
        light_color.a = 1.f;
        light_color.r = sinf(time_info.current_time * 2.0f);
        light_color.g = sinf(time_info.current_time * 0.7f);
        light_color.b = sinf(time_info.current_time * 1.3f);
        
        v4 diffuse_color = light_color * make_v4(0.5f, 0.5f, 0.5f, 1.f);
        v4 ambient_color = diffuse_color * make_v4(0.2f, 0.2f, 0.2f, 1.f);
#else
        v4 ambient_color = make_v4(.2f, .2f, .2f, 1.f);
        v4 diffuse_color = make_v4(.5f, .5f, .5f, 1.f);
#endif
        
        for (sokoban_entity *entity = state->entities; entity != state->entities + array_count(state->entities); entity++) {
            
            if (entity->kind == SokobanEntityKind_None) {
                continue;
            }
            
            mat4 model_matrix = translate(entity->position);
            
            switch (entity->kind) {
                case SokobanEntityKind_Tile: {
                    // TODO(diego): Specific tile stuff
                    set_shader(global_basic_3d_shader);
                    
                    set_float3("light.position", light->position);
                    //set_float4("light.direction", make_v4(1.f, 0.f, .0f. .0f));
                    set_float4("light.ambient", ambient_color);
                    set_float4("light.diffuse", diffuse_color);
                    set_float4("light.specular", make_v4(1.0f, 1.0f, 1.0f, 1.f));
                    
                    set_float("light.constant", 1.f);
                    set_float("light.linear", .09f);
                    set_float("light.quadratic", .032f);
                    
                    // Set material values
                    set_int1("material.diffuse", 0);
                    set_int1("material.specular", 1);
                    set_float("material.shininess", 64.f);
                    
                    set_float3("view_position", state->cam.position);
                    
                } break;
                case SokobanEntityKind_Light: {
                    
                    set_shader(global_basic_3d_light_shader);
                    set_float4("light_color", white);
                    
                    model_matrix = scale(model_matrix, make_v3(.2f, .2f, .2f));
                } break;
                
                default: {
                    continue;
                } break;
            }
            
            immediate_begin();
            
            v3 n0 = make_v3(0.0f,  0.0f, -1.0f);
            immediate_vertex(make_v3(-.5f, -.5f, -.5f), entity->color, make_v2(.0f, .0f), n0);
            immediate_vertex(make_v3( .5f, -.5f, -.5f), entity->color, make_v2(1.f, .0f), n0);
            immediate_vertex(make_v3( .5f,  .5f, -.5f), entity->color, make_v2(1.f, 1.f), n0);
            immediate_vertex(make_v3( .5f,  .5f, -.5f), entity->color, make_v2(1.f, 1.f), n0);
            immediate_vertex(make_v3(-.5f,  .5f, -.5f), entity->color, make_v2(.0f, 1.f), n0);
            immediate_vertex(make_v3(-.5f, -.5f, -.5f), entity->color, make_v2(.0f, .0f), n0);
            
            v3 n1 = make_v3(0.0f,  0.0f,  1.0f);
            immediate_vertex(make_v3(-.5f, -.5f,  .5f), entity->color, make_v2(.0f, .0f), n1);
            immediate_vertex(make_v3( .5f, -.5f,  .5f), entity->color, make_v2(1.f, .0f), n1);
            immediate_vertex(make_v3( .5f,  .5f,  .5f), entity->color, make_v2(1.f, 1.f), n1);
            immediate_vertex(make_v3( .5f,  .5f,  .5f), entity->color, make_v2(1.f, 1.f), n1);
            immediate_vertex(make_v3(-.5f,  .5f,  .5f), entity->color, make_v2(.0f, 1.f), n1);
            immediate_vertex(make_v3(-.5f, -.5f,  .5f), entity->color, make_v2(.0f, .0f), n1);
            
            v3 n2 = make_v3(-1.0f,  0.0f,  0.0f);
            immediate_vertex(make_v3(-.5f,  .5f,  .5f), entity->color, make_v2(1.0f, .0f), n2);
            immediate_vertex(make_v3(-.5f,  .5f, -.5f), entity->color, make_v2(1.f, 1.0f), n2);
            immediate_vertex(make_v3(-.5f, -.5f, -.5f), entity->color, make_v2(0.f, 1.f), n2);
            immediate_vertex(make_v3(-.5f, -.5f, -.5f), entity->color, make_v2(0.f, 1.f), n2);
            immediate_vertex(make_v3(-.5f, -.5f,  .5f), entity->color, make_v2(.0f, 0.f), n2);
            immediate_vertex(make_v3(-.5f,  .5f,  .5f), entity->color, make_v2(1.0f, .0f), n2);
            
            v3 n3 = make_v3(1.0f,  0.0f,  0.0f);
            immediate_vertex(make_v3( .5f,  .5f,  .5f), entity->color, make_v2(1.0f, .0f), n3);
            immediate_vertex(make_v3( .5f,  .5f, -.5f), entity->color, make_v2(1.f, 1.0f), n3);
            immediate_vertex(make_v3( .5f, -.5f, -.5f), entity->color, make_v2(0.f, 1.f), n3);
            immediate_vertex(make_v3( .5f, -.5f, -.5f), entity->color, make_v2(0.f, 1.f), n3);
            immediate_vertex(make_v3( .5f, -.5f,  .5f), entity->color, make_v2(.0f, 0.f), n3);
            immediate_vertex(make_v3( .5f,  .5f,  .5f), entity->color, make_v2(1.0f, .0f), n3);
            
            v3 n4 = make_v3(0.0f,  -1.0f,  0.0f);
            immediate_vertex(make_v3(-.5f, -.5f, -.5f), entity->color, make_v2(.0f, 1.0f), n4);
            immediate_vertex(make_v3( .5f, -.5f, -.5f), entity->color, make_v2(1.f, 1.0f), n4);
            immediate_vertex(make_v3( .5f, -.5f,  .5f), entity->color, make_v2(1.f, 0.f), n4);
            immediate_vertex(make_v3( .5f, -.5f,  .5f), entity->color, make_v2(1.f, 0.f), n4);
            immediate_vertex(make_v3(-.5f, -.5f,  .5f), entity->color, make_v2(.0f, 0.f), n4);
            immediate_vertex(make_v3(-.5f, -.5f, -.5f), entity->color, make_v2(.0f, 1.0f), n4);
            
            v3 n5 = make_v3(0.0f,  1.0f,  0.0f);
            immediate_vertex(make_v3(-.5f,  .5f, -.5f), entity->color, make_v2(.0f, 1.0f), n5);
            immediate_vertex(make_v3( .5f,  .5f, -.5f), entity->color, make_v2(1.f, 1.0f), n5);
            immediate_vertex(make_v3( .5f,  .5f,  .5f), entity->color, make_v2(1.f, 0.f), n5);
            immediate_vertex(make_v3( .5f,  .5f,  .5f), entity->color, make_v2(1.f, 0.f), n5);
            immediate_vertex(make_v3(-.5f,  .5f,  .5f), entity->color, make_v2(.0f, 0.f), n5);
            immediate_vertex(make_v3(-.5f,  .5f, -.5f), entity->color, make_v2(.0f, 1.0f), n5);
            
            set_mat4("model", model_matrix);
            refresh_shader_transform();
            
            immediate_flush();
        }
        
#endif
        
        open_gl->glActiveTexture(GL_TEXTURE0);
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
        state->assets.container = load_texture("./data/textures/sokoban/container.png");
        state->assets.container_specular = load_texture("./data/textures/sokoban/container_specular.png");
        
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
        
        input->mouse.velocity = new_mouse_position - input->mouse.position;
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
