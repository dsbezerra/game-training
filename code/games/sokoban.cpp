
global_variable Mat4 light_space_matrix;
global_variable Vector3 light_pos = make_vector3(-2.f, 4.f, -1.f);
global_variable Vector3 origin = make_vector3(0.f, 0.f, 0.f);
global_variable Vector3 plane_position = make_vector3(origin.x, origin.y - .25f, origin.z);

global_variable Loaded_Sound requiem;

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
    
    init_camera(&state->cam, -90.f, -90.f, 45.f);
    
    set_camera_mode(&state->cam, CameraMode_LookAt);
    platform_show_cursor(true);
    
    state->cam.position  = make_vector3(0.f, 20.f, 0.f);
    state->cam.target = origin;
    
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
    state->arrow = load_mesh("./data/models/sokoban/arrow.obj", MESH_FLIP_UVS);
}

internal void
update_game(Sokoban_State *state, Game_Input *input) {
    
    Camera *cam = &state->cam;
    update_camera(cam, input);
    
    real32 move_step = 0.5f;
    if (cam->mode == CameraMode_LookAt) {
        if (pressed(Button_S)) {
            fade_out(state->requiem);
        }
        if (pressed(Button_W)) {
            fade_in(state->requiem);
        }
        
#if 0
        b32 player_moved = false;
        Vector3 new_player_position = state->player.position;
        if (pressed(Button_W)) {
            new_player_position.z -= move_step;
        }
        if (pressed(Button_S)) {
            new_player_position.z += move_step;
            set_volume(state->requiem, .5f);
        }
        if (pressed(Button_A)) {
            new_player_position.x -= move_step;
            set_volume(state->requiem, .0f);
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
    
    if (pressed(Button_F2)) {
        if (cam->mode == CameraMode_Free) {
            set_camera_mode(cam, CameraMode_LookAt);
            platform_show_cursor(true);
        } else { 
            set_camera_mode(cam, CameraMode_Free);
            platform_show_cursor(false);
        }
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
                entity.position.x -= size_x * .5f - .25f;
                entity.position.z -= size_y * .5f - .25f;
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
        draw_mesh(&state->plane, plane_position, a, 2.f);
    }
    
    //
    // Draw test blocks
    //
    {
        local_persist real32 angle = 0.f;
        angle += core.time_info.dt * 90.f;
        if (angle >= 360.f) angle -= 360.f;
        
        local_persist real32 sun_angle = 0.f;
        sun_angle += core.time_info.dt * 1.f;
        if (sun_angle >= 360.f) sun_angle -= 360.f;
        
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
                    orientation = make_quaternion(make_vector3(0.f, 1.f, .0f), angle);
                    entity.position.y += sinf(core.time_info.current_time * 2.f) * .02f;
                } else if (entity.kind == SokobanEntityKind_Sun) {
                    orientation = make_quaternion(make_vector3(0.f, 1.f, .0f), sun_angle);
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
    
    use_framebuffer(&immediate->depth_framebuffer);
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
draw_grid(Sokoban_State *state) {
    
    set_shader(global_line_shader);
    refresh_shader_transform();
    
    Vector4 pink = make_color(0xffff00ff);
    Vector4 white = lerp_color(make_color(0x00ffffff), 1.f, make_color(0xffffffff));
    
    u32 xc = state->world.x_count;
    u32 yc = state->world.y_count;
    
    int max_count = (int) (xc > yc ? xc : yc);
    
    real32 min_x = (real32) -max_count * .5f;
    real32 max_x = (real32) max_count  * .5f;
    
    real32 min_y = (real32) -max_count * .5f;
    real32 max_y = (real32) max_count  * .5f;
    
    immediate_begin();
    real32 ground = -0.24f;
    for (int x = -max_count; x < max_count; x++) {
        Vector3 p0 = make_vector3(x * .5f + .25f, ground, min_y);
        Vector3 p1 = make_vector3(x * .5f + .25f, ground, max_y);
        immediate_line(p0, p1, white);
    }
    
    for (int y = -max_count; y < max_count; y++) {
        Vector3 p0 = make_vector3(min_x, ground, y * .5f + .25f);
        Vector3 p1 = make_vector3(max_x, ground, y * .5f + .25f);
        immediate_line(p0, p1, white);
    }
    
#define DRAW_ORIGIN_AXIS 1
#if DRAW_ORIGIN_AXIS
    // Origin axises
    Vector4 xcolor = make_color(0xffc80000);
    Vector4 ycolor = make_color(0xff00c800);
    Vector4 zcolor = make_color(0xff0000c8);
    
    real32 len = (real32) max_count * .5f;
    
    Vector3 xs = make_vector3(-len, 0.f, 0.0f);
    Vector3 xe = make_vector3(len, 0.f, 0.0f);
    
    Vector3 ys = make_vector3(0.f, -len, 0.f);
    Vector3 ye = make_vector3(0.f, len, 0.f);
    
    Vector3 zs = make_vector3(0.f, 0.0f, -len);
    Vector3 ze = make_vector3(0.f, 0.0f, len);
    
    immediate_line(xs, xe, xcolor);
    immediate_line(ys, ye, ycolor);
    immediate_line(zs, ze, zcolor);
    immediate_flush();
    
    set_shader(global_arrow_shader);
    refresh_shader_transform();
    
    real32 arrow_scale = 0.1f;
    
    Vector3 x_axis = make_vector3(1.0f, 0.0f, 0.f);
    Vector3 y_axis = make_vector3(0.0f, 1.0f, 0.f);
    Vector3 z_axis = make_vector3(0.0f, 0.0f, 1.f);
    
    // X arrows
    {
        Quaternion x0 = make_quaternion(z_axis, 90.f);
        Quaternion x1 = make_quaternion(z_axis, -90.f);
        
        set_float4("diffuse_color", xcolor);
        
        draw_mesh(&state->arrow, xs, x0, arrow_scale);
        draw_mesh(&state->arrow, xe, x1, arrow_scale);
    }
    
    // Y arrows
    {
        Quaternion y0 = make_quaternion(x_axis, -180.f);
        Quaternion y1 = quaternion_identity();
        
        set_float4("diffuse_color", ycolor);
        
        draw_mesh(&state->arrow, ys, y0, arrow_scale);
        draw_mesh(&state->arrow, ye, y1, arrow_scale);
    }
    // Z arrows
    {
        Quaternion z0 = make_quaternion(x_axis, -90.f);
        Quaternion z1 = make_quaternion(x_axis,  90.f);
        
        set_float4("diffuse_color", zcolor);
        
        draw_mesh(&state->arrow, zs, z0, arrow_scale);
        draw_mesh(&state->arrow, ze, z1, arrow_scale);
    }
#endif
}

internal void
draw_game_view(Sokoban_State *state) {
    if (state->Game_Mode == GameMode_Playing) {
        
        draw_game_shadow(state);
        
        int width = state->dimensions.width;
        int height = state->dimensions.height;
        ensure_framebuffer(width, height);
        use_framebuffer(&immediate->multisampled_framebuffer);
        
        render_3d(width, height, state->cam.fov);
        glClearColor(0.f/255.f, 170.f/255.f, 255.f/255.f, 255.f/255.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //
        // Draw grid
        //
        
        view_matrix = get_view_matrix(&state->cam);
        
        draw_grid(state);
        
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
    
    if (state->cam.mode == CameraMode_Free)
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