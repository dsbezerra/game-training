internal void
init_game(Sokoban_State *state) {
    state->Game_Mode = GameMode_Playing;
    init_camera(&state->cam);
    set_camera_mode(&state->cam, CameraMode_Free);
    
    
    state->cam.position = make_vector3(.0f, .0f, 3.0f);
    
    Vector4 white = make_color(0xffffffff);
    
    s32 stride = array_count(state->entities) / 16;
    
    s32 cube_rows = 5;
    
    s32 tile_x = 0;
    s32 tile_y = 0;
    
    
    for (int i = 2; i < 3; i++) {
        
        Sokoban_Entity *entity = &state->entities[i];
        
        entity->kind = SokobanEntityKind_Tile;
        
        Vector3 p = make_vector3(tile_x * .5f, .0f, tile_y * 0.5f);
        entity->position.x = p.x;
        entity->position.y = p.y;
        entity->position.z = p.z;
        
        entity->color = make_vector4(1.0f, 1.0f, 1.0f, 1.f);
        if (tile_x >= cube_rows) {
            tile_x = 0;
            tile_y++;
        }
        
        tile_x++;
    }
    
    Vector3 point_light_positions[] = {
        make_vector3( 0.7f,  0.2f,  8.0f),
        make_vector3( 4.3f, -3.3f, -4.0f),
    };
    
    Sokoban_Entity *first_light  = &state->entities[0];
    Sokoban_Entity *second_light = &state->entities[1];
    
    first_light->kind = SokobanEntityKind_Light;
    first_light->position = point_light_positions[0];
    first_light->color = make_color(0xffffffff);
    
    second_light->kind = SokobanEntityKind_Light;
    second_light->position = point_light_positions[1];
    second_light->color = make_color(0xffff00ff);
    
    
    //
    // Player
    //
    Sokoban_Player player = {};
    player.position = make_vector3(0.f, 0.f, 0.f);
    player.velocity = make_vector3(0.f, 0.f, 0.f);
    
    //
    // Load mesh
    //
    player.mesh = load_mesh("./data/models/colored_cube.obj");
    
    state->player = player;
}

internal void
update_game(Sokoban_State *state, Game_Input *input) {
    // TODO
    update_camera(&state->cam, input);
}

internal void
draw_tile(Sokoban_State *state) {
    set_shader(global_basic_3d_shader);
    
    set_float3("dir_light.ambient", make_vector3(0.05f, 0.05f, 0.05f));
    set_float3("dir_light.diffuse", make_vector3(0.4f, 0.4f, 0.4f));
    set_float3("dir_light.specular", make_vector3(0.5f, 0.5f, 0.5f));
    set_float3("dir_light.direction", make_vector3(-0.2f, -1.0f, -0.3f));
    
    
    Vector4 color = state->entities[0].color;
    set_float3("point_lights[0].position", state->entities[0].position);
    set_float3("point_lights[0].ambient", make_vector3(0.05f, 0.05f, 0.05f));
    set_float3("point_lights[0].diffuse", make_vector3(0.8f, 0.8f, 0.8f));
    set_float3("point_lights[0].specular", make_vector3(color.r, color.g, color.b));
    set_float("point_lights[0].constant", 1.f);
    set_float("point_lights[0].linear", .09f);
    set_float("point_lights[0].quadratic", .0032f);
    
    color = state->entities[1].color;
    set_float3("point_lights[1].position", state->entities[1].position);
    set_float3("point_lights[1].ambient", make_vector3(0.05f, 0.05f, 0.05f));
    set_float3("point_lights[1].diffuse", make_vector3(0.8f, 0.8f, 0.8f));
    set_float3("point_lights[1].specular", make_vector3(color.r, color.g, color.b));
    set_float("point_lights[1].constant", 1.f);
    set_float("point_lights[1].linear", .09f);
    set_float("point_lights[1].quadratic", .0032f);
    
    
    // Set material values
    
    set_float3("view_position", state->cam.position);
}

internal void
draw_game_view(Sokoban_State *state) {
    if (state->Game_Mode == GameMode_Playing) {
        // TODO
#define TEST_3D 1
#if TEST_3D
        
        render_3d(state->dimensions.width, state->dimensions.height, state->cam.fov);
        
        glClearColor(.2f, .3f, 0.2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        Vector4 white = make_color(0xffffffff);
        view_matrix = get_view_matrix(&state->cam);
        
        Sokoban_Entity *light = &state->entities[0];
        
        
#if 0
        open_gl->glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, state->assets.container);
        
        open_gl->glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, state->assets.container_specular);
        
        for (Sokoban_Entity *entity = state->entities; entity != state->entities + array_count(state->entities); entity++) {
            
            if (entity->kind == SokobanEntityKind_None) {
                continue;
            }
            
            Mat4 model_matrix = translate(entity->position);
            
            switch (entity->kind) {
                case SokobanEntityKind_Tile: {
                    // TODO(diego): Specific tile stuff
                    draw_tile(state);
                } break;
                case SokobanEntityKind_Light: {
                    
                    set_shader(global_basic_3d_light_shader);
                    set_float4("light_color", white);
                    
                    model_matrix = scale(model_matrix, make_vector3(.2f, .2f, .2f));
                } break;
                
                default: {
                    continue;
                } break;
            }
            
            immediate_begin();
            
            Vector3 n0 = make_vector3(0.0f,  0.0f, -1.0f);
            immediate_vertex(make_vector3(-.5f, -.5f, -.5f), entity->color, make_vector2(.0f, .0f), n0);
            immediate_vertex(make_vector3( .5f, -.5f, -.5f), entity->color, make_vector2(1.f, .0f), n0);
            immediate_vertex(make_vector3( .5f,  .5f, -.5f), entity->color, make_vector2(1.f, 1.f), n0);
            immediate_vertex(make_vector3( .5f,  .5f, -.5f), entity->color, make_vector2(1.f, 1.f), n0);
            immediate_vertex(make_vector3(-.5f,  .5f, -.5f), entity->color, make_vector2(.0f, 1.f), n0);
            immediate_vertex(make_vector3(-.5f, -.5f, -.5f), entity->color, make_vector2(.0f, .0f), n0);
            
            Vector3 n1 = make_vector3(0.0f,  0.0f,  1.0f);
            immediate_vertex(make_vector3(-.5f, -.5f,  .5f), entity->color, make_vector2(.0f, .0f), n1);
            immediate_vertex(make_vector3( .5f, -.5f,  .5f), entity->color, make_vector2(1.f, .0f), n1);
            immediate_vertex(make_vector3( .5f,  .5f,  .5f), entity->color, make_vector2(1.f, 1.f), n1);
            immediate_vertex(make_vector3( .5f,  .5f,  .5f), entity->color, make_vector2(1.f, 1.f), n1);
            immediate_vertex(make_vector3(-.5f,  .5f,  .5f), entity->color, make_vector2(.0f, 1.f), n1);
            immediate_vertex(make_vector3(-.5f, -.5f,  .5f), entity->color, make_vector2(.0f, .0f), n1);
            
            Vector3 n2 = make_vector3(-1.0f,  0.0f,  0.0f);
            immediate_vertex(make_vector3(-.5f,  .5f,  .5f), entity->color, make_vector2(1.0f, .0f), n2);
            immediate_vertex(make_vector3(-.5f,  .5f, -.5f), entity->color, make_vector2(1.f, 1.0f), n2);
            immediate_vertex(make_vector3(-.5f, -.5f, -.5f), entity->color, make_vector2(0.f, 1.f), n2);
            immediate_vertex(make_vector3(-.5f, -.5f, -.5f), entity->color, make_vector2(0.f, 1.f), n2);
            immediate_vertex(make_vector3(-.5f, -.5f,  .5f), entity->color, make_vector2(.0f, 0.f), n2);
            immediate_vertex(make_vector3(-.5f,  .5f,  .5f), entity->color, make_vector2(1.0f, .0f), n2);
            
            Vector3 n3 = make_vector3(1.0f,  0.0f,  0.0f);
            immediate_vertex(make_vector3( .5f,  .5f,  .5f), entity->color, make_vector2(1.0f, .0f), n3);
            immediate_vertex(make_vector3( .5f,  .5f, -.5f), entity->color, make_vector2(1.f, 1.0f), n3);
            immediate_vertex(make_vector3( .5f, -.5f, -.5f), entity->color, make_vector2(0.f, 1.f), n3);
            immediate_vertex(make_vector3( .5f, -.5f, -.5f), entity->color, make_vector2(0.f, 1.f), n3);
            immediate_vertex(make_vector3( .5f, -.5f,  .5f), entity->color, make_vector2(.0f, 0.f), n3);
            immediate_vertex(make_vector3( .5f,  .5f,  .5f), entity->color, make_vector2(1.0f, .0f), n3);
            
            Vector3 n4 = make_vector3(0.0f,  -1.0f,  0.0f);
            immediate_vertex(make_vector3(-.5f, -.5f, -.5f), entity->color, make_vector2(.0f, 1.0f), n4);
            immediate_vertex(make_vector3( .5f, -.5f, -.5f), entity->color, make_vector2(1.f, 1.0f), n4);
            immediate_vertex(make_vector3( .5f, -.5f,  .5f), entity->color, make_vector2(1.f, 0.f), n4);
            immediate_vertex(make_vector3( .5f, -.5f,  .5f), entity->color, make_vector2(1.f, 0.f), n4);
            immediate_vertex(make_vector3(-.5f, -.5f,  .5f), entity->color, make_vector2(.0f, 0.f), n4);
            immediate_vertex(make_vector3(-.5f, -.5f, -.5f), entity->color, make_vector2(.0f, 1.0f), n4);
            
            Vector3 n5 = make_vector3(0.0f,  1.0f,  0.0f);
            immediate_vertex(make_vector3(-.5f,  .5f, -.5f), entity->color, make_vector2(.0f, 1.0f), n5);
            immediate_vertex(make_vector3( .5f,  .5f, -.5f), entity->color, make_vector2(1.f, 1.0f), n5);
            immediate_vertex(make_vector3( .5f,  .5f,  .5f), entity->color, make_vector2(1.f, 0.f), n5);
            immediate_vertex(make_vector3( .5f,  .5f,  .5f), entity->color, make_vector2(1.f, 0.f), n5);
            immediate_vertex(make_vector3(-.5f,  .5f,  .5f), entity->color, make_vector2(.0f, 0.f), n5);
            immediate_vertex(make_vector3(-.5f,  .5f, -.5f), entity->color, make_vector2(.0f, 1.0f), n5);
            
            set_Mat4("model", model_matrix);
            refresh_shader_transform();
            
            immediate_flush();
        }
#else
        
        //
        // Draw player
        //
        
        draw_tile(state);
        
        Mat4 model_matrix = translate(state->player.position);
        
        local_persist real32 angle = time_info.dt;
        
        model_matrix = y_rotation(angle_to_radians(angle));
        set_Mat4("model", model_matrix);
        
        angle += time_info.dt * 30.f;
        if (angle > 360.f) {
            angle -= 360.f;
        }
        
        refresh_shader_transform();
        
        draw_mesh(&state->player.mesh);
        
#endif
#endif
        
        open_gl->glActiveTexture(GL_TEXTURE0);
    } else {
        draw_menu(SOKOBAN_TITLE, state->dimensions, state->Game_Mode, state->menu_selected_item, state->quit_was_selected);
    }
}

internal void
sokoban_menu_art(App_State *state, Vector2 min, Vector2 max) {
    Vector4 background = make_color(0xFFFF00FF);
    immediate_begin();
    immediate_quad(min, max, background);
    immediate_flush();
}

internal void
sokoban_game_restart(Sokoban_State *state) {
    init_game(state);
}

internal void
sokoban_game_update_and_render(Game_Memory *memory, Game_Input *input) {
    Sokoban_State *state = (Sokoban_State *) memory->permanent_storage;
    if (!memory->initialized) {
        assert(memory->permanent_storage_size == 0);
        assert(!memory->permanent_storage);
        memory->initialized = true;
        
        state = (Sokoban_State *) game_alloc(memory, megabytes(12));
        state->assets.none = load_texture("./data/textures/sokoban/none.jpg");
        state->assets.container = load_texture("./data/textures/sokoban/container.png");
        state->assets.container_specular = load_texture("./data/textures/sokoban/container_specular.png");
        
        state->cam.position = make_vector3(.0f, 0.f, -3.f);
        
        init_game(state);
        
        // TODO(diego): Better way to do this
        input->mouse.sensitivity = 0.05f;
    }
    state->dimensions = memory->window_dimensions;
    
    // NOTE(diego): Lock mouse to center of screen and use new position
    // to calculate velocity and move our player with it.
    {
        Vector2i new_mouse_position;
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
    if (state->Game_Mode == GameMode_Playing) {
        if (pressed(Button_Escape)) {
            state->Game_Mode = GameMode_Menu;
        } else {
            update_game(state, input);
        }
    } else if (state->Game_Mode == GameMode_Menu ||
               state->Game_Mode == GameMode_GameOver) {
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
