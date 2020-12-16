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
    
    
    for (int i = 2; i < 3; i++) {
        
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
    
    v3 point_light_positions[] = {
        make_v3( 0.7f,  0.2f,  8.0f),
        make_v3( 4.3f, -3.3f, -4.0f),
    };
    
    sokoban_entity *first_light  = &state->entities[0];
    sokoban_entity *second_light = &state->entities[1];
    
    first_light->kind = SokobanEntityKind_Light;
    first_light->position = point_light_positions[0];
    first_light->color = make_color(0xffffffff);
    
    second_light->kind = SokobanEntityKind_Light;
    second_light->position = point_light_positions[1];
    second_light->color = make_color(0xffff00ff);
    
    
    //
    // Player
    //
    sokoban_player player = {};
    player.position = make_v3(0.f, 0.f, 0.f);
    player.velocity = make_v3(0.f, 0.f, 0.f);
    
    //
    // Load mesh
    //
    player.model = load_model("./data/models/cube_t.obj");
    
    state->player = player;
}

internal void
update_game(sokoban_state *state, game_input *input) {
    // TODO
    update_camera(&state->cam, input);
}

internal void
draw_tile(sokoban_state *state) {
    set_shader(global_basic_3d_shader);
    
    set_float3("dir_light.ambient", make_v3(0.05f, 0.05f, 0.05f));
    set_float3("dir_light.diffuse", make_v3(0.4f, 0.4f, 0.4f));
    set_float3("dir_light.specular", make_v3(0.5f, 0.5f, 0.5f));
    set_float3("dir_light.direction", make_v3(-0.2f, -1.0f, -0.3f));
    
    
    set_float3("point_lights[0].position", state->entities[0].position);
    set_float3("point_lights[0].ambient", make_v3(0.05f, 0.05f, 0.05f));
    set_float3("point_lights[0].diffuse", make_v3(0.8f, 0.8f, 0.8f));
    set_float3("point_lights[0].specular", make_v3(1.0f, 1.0f, 1.0f));
    set_float("point_lights[0].constant", 1.f);
    set_float("point_lights[0].linear", .09f);
    set_float("point_lights[0].quadratic", .0032f);
    
    set_float3("point_lights[1].position", state->entities[1].position);
    set_float3("point_lights[1].ambient", make_v3(0.05f, 0.05f, 0.05f));
    set_float3("point_lights[1].diffuse", make_v3(0.8f, 0.8f, 0.8f));
    set_float3("point_lights[1].specular", make_v3(1.0f, 1.0f, 1.0f));
    set_float("point_lights[1].constant", 1.f);
    set_float("point_lights[1].linear", .09f);
    set_float("point_lights[1].quadratic", .0032f);
    
    
    // Set material values
    set_int1("material.diffuse", 0);
    set_int1("material.specular", 1);
    set_float("material.shininess", 64.f);
    
    set_float3("view_position", state->cam.position);
}

internal void
draw_game_view(sokoban_state *state) {
    if (state->game_mode == GameMode_Playing) {
        // TODO
#define TEST_3D 1
#if TEST_3D
        
        render_3d(state->dimensions.width, state->dimensions.height, state->cam.fov);
        
        glClearColor(.2f, .3f, 0.2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        v4 white = make_color(0xffffffff);
        view_matrix = get_view_matrix(&state->cam);
        
        sokoban_entity *light = &state->entities[0];
        
        
#if 0
        open_gl->glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, state->assets.container);
        
        open_gl->glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, state->assets.container_specular);
        
        for (sokoban_entity *entity = state->entities; entity != state->entities + array_count(state->entities); entity++) {
            
            if (entity->kind == SokobanEntityKind_None) {
                continue;
            }
            
            mat4 model_matrix = translate(entity->position);
            
            switch (entity->kind) {
                case SokobanEntityKind_Tile: {
                    // TODO(diego): Specific tile stuff
                    draw_tile(state);
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
        
        //
        // Draw player
        //
        
        set_shader(global_basic_3d_light_shader);
        
        mat4 model_matrix = translate(state->player.position);
        set_mat4("model", model_matrix);
        refresh_shader_transform();
        
        draw_model(&state->player.model);
        
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
