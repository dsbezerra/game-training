
        global_variable Vector3 origin = make_vector3(0.f, 0.f, 0.f);
global_variable Vector3 plane_position = make_vector3(origin.x, origin.y - .01f, origin.z);

global_variable Loaded_Sound violin, test;
global_variable int blinn = 0;

internal Sokoban_Entity
make_block(Vector3 position) {
    Sokoban_Entity result = {};
    result.kind = SokobanEntityKind_Block;
    result.position = position;
    return result;
}

internal Sokoban_Entity
make_star(Vector3 position) {
    Sokoban_Entity result = {};
    result.kind = SokobanEntityKind_Star;
    result.position = position;
    return result;
}

internal void
init_game(Sokoban_State *state) {
    state->violin = play_sound("Violin", &violin);
    
    state->Game_Mode = GameMode_Playing;
    
    init_camera(&state->cam, -90.f, -65.f, 45.f);
    set_camera_mode(&state->cam, CameraMode_LookAt);
    
    state->cam.position  = make_vector3(0.f, 5.f, 4.f);
    
    Sokoban_World world = {};
    world.x_count = SOKOBAN_WORLD_X;
    world.y_count = SOKOBAN_WORLD_Y;
    world.num_entities = world.x_count * world.y_count;
    world.entities = (Sokoban_Entity *) platform_alloc(world.num_entities * sizeof(Sokoban_Entity));
    state->world = world;
    
    //
    // Blocks
    //
    real32 block_size = 0.5f;
    real32 bb = block_size / 2.f;
    
    u32 idx = 0;
    
    // Last row
    world.entities[idx++] = make_block(make_vector3(0.0f, bb, 2.0f));
    world.entities[idx++] = make_block(make_vector3(0.5f, bb, 2.0f));
    world.entities[idx++] = make_block(make_vector3(1.0f, bb, 2.0f));
    
    world.entities[idx++] = make_block(make_vector3(-0.5f, bb, 1.5f));
    world.entities[idx++] = make_block(make_vector3(-0.5f, bb, 1.0f));
    
    world.entities[idx++] = make_block(make_vector3(-1.0f, bb, 0.5f));
    world.entities[idx++] = make_block(make_vector3(-1.5f, bb, 0.5f));
    
    // left wall
    world.entities[idx++] = make_block(make_vector3(-2.0f, bb, 0.0f));
    world.entities[idx++] = make_block(make_vector3(-2.0f, bb, -0.5f));
    world.entities[idx++] = make_block(make_vector3(-2.0f, bb, -1.0f));
    
    // right wall
    world.entities[idx++] = make_block(make_vector3(2.0f, bb,  1.0f));
    world.entities[idx++] = make_block(make_vector3(2.0f, bb,  0.5f));
    world.entities[idx++] = make_block(make_vector3(2.0f, bb,  0.0f));
    world.entities[idx++] = make_block(make_vector3(2.0f, bb, -0.5f));
    world.entities[idx++] = make_block(make_vector3(2.0f, bb, -1.0f));
    world.entities[idx++] = make_block(make_vector3(2.0f, bb, -1.5f));
    
    // top wall
    world.entities[idx++] = make_block(make_vector3(-1.0f, bb, -2.0f));
    world.entities[idx++] = make_block(make_vector3(-0.5f, bb, -2.0f));
    world.entities[idx++] = make_block(make_vector3(0.0f, bb, -2.0f));
    world.entities[idx++] = make_block(make_vector3(0.5f, bb, -2.0f));
    world.entities[idx++] = make_block(make_vector3(1.0f, bb, -2.0f));
    world.entities[idx++] = make_block(make_vector3(1.5f, bb, -2.0f));
    
    // Star
    world.entities[idx++] = make_star(make_vector3(0.f, 0.15f, 1.f));
    
    
    //
    // Load meshes
    //
    Vector3 plane_color = make_vector3(206.f/255.f, 209.f/255.f, 200.f/255.f);
    Vector3 block_color = make_vector3(180.f/255.f, 118.f/255.f, 61.f/255.f);
    
    state->block = load_mesh("./data/models/sokoban/block.obj", MESH_FLIP_UVS);
    state->star  = load_mesh("./data/models/sokoban/star.obj", MESH_FLIP_UVS);
    state->plane = load_mesh("./data/models/sokoban/plane.obj", MESH_FLIP_UVS);
    
    //
    // Player
    //
    Sokoban_Player player = {};
    player.position = make_vector3(0.f, 0.3f, 0.f);
    player.velocity = make_vector3(0.f, 0.f, 0.f);
    player.mesh = load_mesh("./data/models/sokoban/cylinder.obj", MESH_FLIP_UVS);
    
    state->player = player;
}

internal void
update_game(Sokoban_State *state, Game_Input *input) {
    // TODO
    Camera *cam = &state->cam;
    update_camera(cam, input);
    
    real32 move_step = 0.5f;
    if (cam->mode == CameraMode_LookAt) {
        b32 player_moved = false;
        Vector3 new_player_position = state->player.position;
        if (pressed(Button_W)) {
            new_player_position.z -= move_step;
        }
        if (pressed(Button_S)) {
            new_player_position.z += move_step;
        }
        if (pressed(Button_A)) {
            new_player_position.x -= move_step;
        }
        if (pressed(Button_D)) {
            new_player_position.x += move_step;
        }
        
        if (new_player_position != state->player.position) {
            b32 allow_move = true;
            for (u32 i = 0; i < state->world.num_entities; ++i) {
                Sokoban_Entity entity = state->world.entities[i];
                if (entity.kind == SokobanEntityKind_Block) {
                    if (entity.position.x == new_player_position.x && entity.position.z == new_player_position.z) {
                        allow_move = false;
                        break;
                    }
                }
            }
            if (allow_move) {
                state->player.position = new_player_position;
            }
        }
    }
    
    if (is_down(Button_Space)) {
        blinn = 0;
    } else {
        blinn = 1;
    }
    
    if (pressed(Button_Space)) {
        if (state->test) {
            restart_sound(state->test);
        } else {
            state->test = play_sound("Short", &test, false);
        }
    }
    
    if (released(Button_Enter)) {
        if (cam->mode == CameraMode_Free) 
            set_camera_mode(cam, CameraMode_LookAt);
        else 
            set_camera_mode(cam, CameraMode_Free);
    }
}

internal void
draw_game_view(Sokoban_State *state) {
    if (state->Game_Mode == GameMode_Playing) {
        
        render_3d(state->dimensions.width, state->dimensions.height, state->cam.fov);
        
        glClearColor(0.f/255.f, 170.f/255.f, 255.f/255.f, 255.f/255.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        view_matrix = get_view_matrix(&state->cam);
        
        //
        // Draw player
        //
        
        set_shader(global_basic_3d_shader);
        refresh_shader_transform();
        
        set_float3("view_position", state->cam.position);
        
        set_int1("blinn", blinn);
        
        // Draw plane
        {
            set_mat4("model", translate(plane_position));
            draw_mesh(&state->plane);
        }
        
        //
        // Draw test blocks
        //
        {
            for (u32 i = 0; i < state->world.num_entities; ++i) {
                Sokoban_Entity entity = state->world.entities[i];
                if (entity.kind == SokobanEntityKind_None) continue;
                
                
                Triangle_Mesh *mesh = 0;
                switch (entity.kind) {
                    case SokobanEntityKind_Block: mesh = &state->block; break; 
                    case SokobanEntityKind_Star:  mesh = &state->star;  break;
                    default: break;
                }
                if (mesh) {
                    Mat4 model_matrix = identity();
                    
                    local_persist real32 angle = core.time_info.dt;
                    angle += core.time_info.dt * 6.f;
                    if (angle > 360.f) {
                        angle -= 360.f;
                    }
                    
                    if (entity.kind == SokobanEntityKind_Star) {
                        model_matrix = y_rotation(angle_to_radians(angle));
                        entity.position.y += sinf(core.time_info.current_time*2.f) * .02f;
                    }
                    
                    model_matrix = model_matrix * translate(entity.position);
                    
                    set_mat4("model", model_matrix);
                    draw_mesh(mesh);
                }
            }
        }
        
        {
            local_persist real32 angle = core.time_info.dt;
            
            Mat4 model_matrix = translate(state->player.position);
            set_mat4("model", model_matrix);
            
            angle += core.time_info.dt * 30.f;
            if (angle > 360.f) {
                angle -= 360.f;
            }
            draw_mesh(&state->player.mesh);
        }
        open_gl->glActiveTexture(GL_TEXTURE0);
        
        draw_camera_debug(&state->cam, state->dimensions);
        
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
        
        violin = load_sound("./data/sounds/violin.wav");
        test = load_sound("./data/sounds/short.wav");
        
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

internal void
sokoban_game_free(Game_Memory *memory) {
    if (!memory) return;
    
    Sokoban_State *state = (Sokoban_State *) memory->permanent_storage;
    if (!state) return;
    
    if (state->violin) state->violin->flags &= ~PLAYING_SOUND_ACTIVE;
    if (state->test) state->test->flags &= ~PLAYING_SOUND_ACTIVE;
}