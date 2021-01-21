
global_variable Mat4 light_space_matrix;
global_variable Vector3 light_pos = make_vector3(-2.f, 4.f, -2.f);
global_variable Vector3 origin = make_vector3(0.f, 0.f, 0.f);
global_variable Vector3 plane_position = make_vector3(origin.x, origin.y - .25f, origin.z);

global_variable Loaded_Sound requiem, test;
global_variable int blinn = 0;

internal Sokoban_Entity
make_entity(Sokoban_Entity_Kind kind, u32 tile_x, u32 tile_y) {
    Sokoban_Entity result = {};
    result.kind = kind;
    result.tile_x = tile_x;
    result.tile_y = tile_y;
    
    Vector3 position = {};
    position.x = (real32) tile_x * .5f;
    position.y = (real32) origin.y;
    position.z = (real32) tile_y * .5f;
    
    result.position = position;
    
    return result;
}

internal void
init_game(Sokoban_State *state) {
    state->requiem = play_sound("Requiem", &requiem);
    set_volume(state->requiem, .1f);
    
    state->Game_Mode = GameMode_Playing;
    
    init_camera(&state->cam, -90.f, -65.f, 45.f);
    set_camera_mode(&state->cam, CameraMode_LookAt);
    
    state->cam.position  = make_vector3(0.f, 5.f, 4.f);
    
    Sokoban_World world = load_level("sasquatch_v_1");
    state->world = world;
    
    //
    // Blocks
    //
    
#if 0
    
    Sokoban_World world = {};
    world.x_count = SOKOBAN_WORLD_X;
    world.y_count = SOKOBAN_WORLD_Y;
    world.num_entities = world.x_count * world.y_count;
    world.entities = (Sokoban_Entity *) platform_alloc(world.num_entities * sizeof(Sokoban_Entity));
    
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
    world.entities[idx++] = make_sun(light_pos);
#endif 
    
    //
    // Load meshes
    //
    Vector3 plane_color = make_vector3(206.f/255.f, 209.f/255.f, 200.f/255.f);
    Vector3 block_color = make_vector3(180.f/255.f, 118.f/255.f, 61.f/255.f);
    
    state->block = load_mesh("./data/models/sokoban/block.obj", MESH_FLIP_UVS);
    state->star  = load_mesh("./data/models/sokoban/star.obj", MESH_FLIP_UVS);
    state->plane = load_mesh("./data/models/sokoban/plane.obj", MESH_FLIP_UVS);
    state->sun   = load_mesh("./data/models/sokoban/sun.obj", MESH_FLIP_UVS);
    
    //
    // Player
    //
#if 0
    Sokoban_Player player = {};
    player.position = make_vector3(0.f, 0.3f, 0.f);
    player.velocity = make_vector3(0.f, 0.f, 0.f);
    player.mesh = load_mesh("./data/models/sokoban/cylinder.obj", MESH_FLIP_UVS);
    state->player = player;
#endif
}

internal void
update_game(Sokoban_State *state, Game_Input *input) {
    
    Camera *cam = &state->cam;
    update_camera(cam, input);
    
    real32 move_step = 0.5f;
    if (cam->mode == CameraMode_LookAt) {
        
#if 0
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
#endif
    }
    
    if (is_down(Button_Space)) {
        blinn = 0;
    } else {
        blinn = 1;
    }
    
    if (released(Button_Enter)) {
        if (cam->mode == CameraMode_Free) 
            set_camera_mode(cam, CameraMode_LookAt);
        else 
            set_camera_mode(cam, CameraMode_Free);
    }
}

internal Sokoban_World
load_level(char *levelname) {
    
    Sokoban_World result = {};
    
    char *folder = "./data/levels/sokoban/";
    char *lvl = ".lvl";
    
    char *filename = concat(levelname, lvl, string_length(levelname), string_length(lvl));
    char *filepath = concat(folder, filename, string_length(folder), string_length(filename));
    
    platform_free(filename);
    
    File_Contents level = platform_read_entire_file(filepath);
    assert(level.file_size > 0);
    platform_free(filepath);
    
    u8 *at = level.contents;
    
    u32 x_count = 0;
    while (*at) {
        if (*at == '\r') {
            // No-op
        }
        else if (*at == '\n') {
            result.y_count++;
            if (x_count > result.x_count) {
                result.x_count = x_count;
            }
            x_count = 0;
        }
        // We say empty spaces are entities for now.
        else {
            x_count++;
        }
        at++;
    }
    
    result.num_entities += result.x_count*result.y_count;
    assert(result.num_entities > 0);
    
    result.entities = (Sokoban_Entity *) platform_alloc(result.num_entities);
    
    at = level.contents;
    
    u32 entity_id = 0;
    
    int xx = 0;
    int yy = 0;
    while (*at) {
        if (*at == '\r') {
            // No-op
        }
        else if (*at == '\n') {
            yy++;
            xx = 0;
        }
        // We say empty spaces are entities for now.
        else {
            Sokoban_Entity_Kind kind = SokobanEntityKind_None;
            if (*at == '#') {
                kind = SokobanEntityKind_Block;
            }
            else if (*at == '$') {
                kind = SokobanEntityKind_Star;
            }
            else if (*at == '.') {
                kind = SokobanEntityKind_Goal;
            }
            else if (*at == '@') {
                kind = SokobanEntityKind_Player;
            }
            if (kind != SokobanEntityKind_None) {
                Sokoban_Entity entity = make_entity(kind, xx, yy);
                real32 size_x = result.x_count * .5f;
                real32 size_y = result.y_count * .5f;
                entity.position.x -= size_x * .5f;
                entity.position.z -= size_y * .5f;
                result.entities[entity_id++] = entity;
            }
            xx++;
        }
        at++;
    }
    
    platform_free(level.contents);
    
    return result;
}

internal void
draw_game_playing(Sokoban_State *state) {
    
    // Draw plane
    Quaternion a = make_quaternion(make_vector3(0.f, 0.f, 0.f), .0f);
    {
        draw_mesh(&state->plane, plane_position - make_vector3(0.2f, 0.f, 0.f), a, 1.4f);
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
                case SokobanEntityKind_Sun:   mesh = &state->sun;   break;
                default: break;
            }
            
            if (mesh) {
                Quaternion orientation = quaternion_identity();
                if (entity.kind == SokobanEntityKind_Star) {
                    local_persist real32 angle = 0.f;
                    orientation = make_quaternion(make_vector3(0.f, 1.f, .0f), angle);
                    angle += core.time_info.dt * 90.f;
                    if (angle >= 360.f) angle -= 360.f;
                    entity.position.y += sinf(core.time_info.current_time * 2.f) * .02f;
                } else if (entity.kind == SokobanEntityKind_Sun) {
                    local_persist real32 sun_angle = 0.f;
                    orientation = make_quaternion(make_vector3(0.f, 1.f, .0f), sun_angle);
                    sun_angle += core.time_info.dt * 1.f;
                    if (sun_angle >= 360.f) sun_angle -= 360.f;
                }
                draw_mesh(mesh, entity.position, orientation);
            }
        }
    }
    
    //
    // Draw player
    //
    {
        //draw_mesh(&state->player.mesh, state->player.position, a);
    }
}

internal void
draw_game_shadow(Sokoban_State *state) {
    if (!immediate->depth_map.fbo) {
        init_depth_map();
    }
    
    int width = immediate->depth_map.width;
    int height = immediate->depth_map.height;
    
    use_framebuffer(&immediate->depth_framebuffer);
    
    glViewport(0, 0, width, height);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    set_shader(global_basic_shadow_shader);
    
    real32 n = 1.f;
    real32 f = 7.5f;
    
    Mat4 projection = ortho(-10.f, 10.f, -10.f, 10.f, n, f);
    Mat4 view = look_at(light_pos,
                        origin,
                        make_vector3(0.f, 1.f, 0.f));
    light_space_matrix = projection * view;
    set_mat4("light_space_matrix", light_space_matrix);
    
    draw_game_playing(state);
}

internal void
draw_game_view(Sokoban_State *state) {
    if (state->Game_Mode == GameMode_Playing) {
        
        draw_game_shadow(state);
        
        int width = state->dimensions.width;
        int height = state->dimensions.height;
        
        ensure_framebuffer(width, height);
        use_framebuffer(&immediate->multisampled_framebuffer);
        
        glViewport(0, 0, width, height);
        
        render_3d(state->dimensions.width, state->dimensions.height, state->cam.fov);
        glClearColor(0.f/255.f, 170.f/255.f, 255.f/255.f, 255.f/255.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        view_matrix = get_view_matrix(&state->cam);
        
        set_shader(global_basic_3d_shader);
        refresh_shader_transform();
        
        set_mat4("light_space_matrix", light_space_matrix);
        set_float3("light_pos", light_pos);
        set_float3("view_position", state->cam.position);
        set_texture("shadow_map", &immediate->depth_map);
        
        draw_game_playing(state);
        draw_camera_debug(&state->cam, state->dimensions);
    } else {
        game_frame_begin(state->dimensions.width, state->dimensions.height);
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
        
        requiem = load_sound("./data/sounds/requiem.wav");
        
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
    
    if (state->requiem) state->requiem->flags &= ~PLAYING_SOUND_ACTIVE;
}