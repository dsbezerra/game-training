
global_variable Mat4 light_space_matrix;
global_variable Vector3 light_pos = make_vector3(-2.f, 4.f, -1.f);
global_variable Vector3 origin = make_vector3(0.f, 0.f, 0.f);
global_variable Vector3 plane_position = make_vector3(origin.x, origin.y - .25f, origin.z);

global_variable Vector3 mouse_ray;
global_variable Vector3 intersect_position;
global_variable Sokoban_Entity *placed_entity = 0;

global_variable Loaded_Sound requiem;

#define LEVEL_CAMERA_OFFSET 1.5f

internal Sokoban_Entity
make_entity(Sokoban_Entity_Kind kind, u32 x, u32 y) {
    Sokoban_Entity result = {};
    result.kind = kind;
    
    Sokoban_World_Position world_position = {};
    world_position.x = x;
    world_position.y = y;
    
    Vector3 position = {};
    position.x = (real32) x;
    position.y = origin.y;
    position.z = (real32) y;
    
    result.world_position = world_position;
    result.position = position;
    result.entity_activated_index = -1;
    result.color = make_color(0xffffffff);
    
    return result;
}

internal void
snap(Vector3 *pos, real32 value) {
    real32 r = 1.f / value;
    pos->x = roundf(pos->x * r) / r;
    pos->y = roundf(pos->y * r) / r;
    pos->z = roundf(pos->z * r) / r;
}

internal void
place_entity(Sokoban_World *world, Sokoban_Entity_Kind kind, Vector3 position) {
    Sokoban_Entity *entity = placed_entity;
    if (!entity) {
        placed_entity = (Sokoban_Entity *) platform_alloc(sizeof(Sokoban_Entity));
        entity = placed_entity;
    }
    entity->kind = kind;
    entity->position = position;
}

internal void
adjust_camera_to_level(Sokoban_State *state, b32 animate) {
    Sokoban_World *world = state->world;
    
    real32 cam_height = (real32) max(world->x_count, world->y_count);
    state->cam_animation_rate = 5.f;
    state->cam.target = origin;
    state->world = world;
    
    // Move camera to lock position.
    if (state->cam.mode == CameraMode_Free) { // Look at is the lock camera.
        // Keep camera where it is.
    } else {
        state->cam.position  = make_vector3(0.f, 0.f, 0.f);
        state->lock_position = state->cam.position + make_vector3(0.f, cam_height * LEVEL_CAMERA_OFFSET, 0.f);
        if (!animate) {
            state->cam.position = state->lock_position;
        }
    }
    
    update_vectors(&state->cam);
}

internal void
init_game(Sokoban_State *state) {
    if (!state->requiem) {
        state->requiem = play_sound("Requiem", &requiem);
    }
    set_volume(state->requiem, .1f);
    
    state->Game_Mode = GameMode_Playing;
    state->lock_pitch = -89.f;
    state->lock_yaw = -90.f;
    state->current_level = 1;
    
    release_sentinel(&state->undo_sentinel);
    release_sentinel(&state->redo_sentinel);
    
    sentinelize(&state->undo_sentinel);
    sentinelize(&state->redo_sentinel);
    
    init_camera(&state->cam, state->lock_yaw, state->lock_pitch, 45.f);
    
    set_camera_mode(&state->cam, CameraMode_LookAt);
    platform_show_cursor(true);
    
    Sokoban_World *world = load_level(state, "sasquatch_v_1");
    assert(world);
    state->world = world;
    state->last_undo_time = 0.f;
    state->last_redo_time = 0.f;
    
    adjust_camera_to_level(state, true);
}

struct AABB {
    Vector3 min;
    Vector3 max;
};

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2

internal b32
clip_line(int axis, AABB& box, Vector3& v0, Vector3& v1, real32& t_low, real32& t_high)
{
    real32 t_dim_low;
    real32 t_dim_high;
    
    t_dim_low  = (box.min.e[axis] - v0.e[axis]) / (v1.e[axis] - v0.e[axis]);
    t_dim_high = (box.max.e[axis] - v0.e[axis]) / (v1.e[axis] - v0.e[axis]);
    
    if (t_dim_high < t_dim_low) {
        real32 aux = t_dim_low;
        t_dim_low = t_dim_high;
        t_dim_high = aux;
    }
    
    if (t_dim_high < t_low) return false;
    
    if (t_dim_low > t_high) return false;
    
    t_low  = max(t_dim_low, t_low);
    t_high = min(t_dim_high, t_high);
    
    if (t_low > t_high) return false;
    
    return true;
}

internal b32
line_aabb_intersection(AABB& box, Vector3& v0, Vector3& v1, Vector3& intersection, real32& t) {
    
    real32 t_low = 0.f;
    real32 t_high = 1.f;
    
    if (!clip_line(X_AXIS, box, v0, v1, t_low, t_high)) return false;
    if (!clip_line(Y_AXIS, box, v0, v1, t_low, t_high)) return false;
    if (!clip_line(Z_AXIS, box, v0, v1, t_low, t_high)) return false;
    
    intersection = v0 + (v1 - v0) * t_low;
    
    t = t_low;
    
    return true;
}

internal b32
line_plane_intersection(Vector3 &n, Vector3& c, Vector3& v0, Vector3& v1, Vector3& intersection, real32& t) {
    // n - plane normal
    // c - any point in plane
    // v0 - the beginning of our line
    // v1 - the end of our line
    
    Vector3 v = v1 - v0;
    Vector3 w = c - v0;
    
    real32 k = -1.f;
    
    real32 denom = inner(v, n);
    if (fabs(denom) > 0.0001f)
    {
        k = inner(w, n) / denom;
        intersection = v0 + k * v;
        t = k;
    }
    
    return k >= 0.f && k <= 1.f;
}

// NOTE(diego): Depends on current set view and projection matrix.
internal Vector3
ray_from_mouse(Vector2i dim) {
    Vector3 result = {};
    
    Vector2i mouse_position;
    platform_get_cursor_position(&mouse_position);
    
    // 1. Normalized device coords
    real32 x = (2.f * mouse_position.x) / dim.width - 1.f;
    real32 y = 1.f - (2.f * mouse_position.y) / dim.height;
    
    // 2. Clip coords
    Vector4 clip = make_vector4(x, y, -1, 1.f);
    
    // 3. Eye coords
    Vector4 eye_coords = inverse(projection_matrix) * clip;
    eye_coords.z = -1.f;
    eye_coords.w =  0.f;
    
    // 4. World coords
    Vector4 world_coords = inverse(view_matrix) * eye_coords;
    
    result.x = world_coords.x;
    result.y = world_coords.y;
    result.z = world_coords.z;
    result = normalize(result);
    
    return result;
}

internal b32
trace_line(Sokoban_World *world, Vector3& v0, Vector3& v1, Vector3& intersection) {
    
    real32 low_t = 1.f;
    
    Vector3 tVec;
    real32 t;
    
    for (u32 i = 0; i < world->num_entities; i++) {
        Sokoban_Entity *entity = &world->entities[i];
        if (entity->kind == SokobanEntityKind_Block) {
            AABB box = {};
            box.min = entity->position - 0.5f;
            box.max = entity->position + 0.5f;
            if (line_aabb_intersection(box, v0, v1, tVec, t) && t < low_t) {
                intersection = tVec;
                low_t = t;
            }
        }
    }
    
    if (low_t < 1) return true;
    
    return false;
}

internal void
print_entity_position(char *name, Sokoban_Entity *e) {
    if (!e) return;
    
    char buf[16];
    sprintf(buf, "%s %d,%d\n", name, e->world_position.x, e->world_position.y);
    OutputDebugString(buf);
}


internal Vector3
to_visual_position(Sokoban_World *world, Sokoban_World_Position world_position, Sokoban_Entity_Kind kind) {
    Vector3 result = {};
    
    real32 tile_size = .25f;
    real32 offset_to_center_at_origin = tile_size;
    
    real32 offset_x = world->x_count * tile_size - offset_to_center_at_origin;
    real32 offset_y = world->y_count * tile_size - offset_to_center_at_origin;
    
    result.x = world_position.x*.5f - offset_x;
    result.z = world_position.y*.5f - offset_y;
    
    if (kind == SokobanEntityKind_Goal) {
        result.y -= 0.25f;
    }
    
    return result;
}

internal b32
are_same_world_position(Sokoban_World_Position *a, Sokoban_World_Position *b) {
    assert(a && b);
    
    b32 result = false;
    
    result = a->x == b->x && a->y == b->y;
    
    return result;
}

internal void
ensure_world_position_is_valid(Sokoban_World *world, Sokoban_World_Position *world_position) {
    world_position->x = clamp(0, world_position->x, world->x_count);
    world_position->y = clamp(0, world_position->y, world->y_count);
}

internal b32
is_empty(Sokoban_Change *list) {
    b32 result = (list->next == list);
    assert((result && (list->prev == list)) ||
           (!result && (list->prev != list)));
    return result;
}

internal b32
undo_available(Sokoban_State *state) {
    b32 result = !is_empty(&state->undo_sentinel);
    return result;
}

internal b32
redo_available(Sokoban_State *state) {
    b32 result = !is_empty(&state->redo_sentinel);
    return result;
}

internal void
link(Sokoban_Change *prev, Sokoban_Change *change) {
    change->prev = prev;
    change->next = prev->next;
    
    change->prev->next = change;
    change->next->prev = change;
}

internal void
sentinelize(Sokoban_Change *change) {
    change->prev = change;
    change->next = change;
}

internal void
release_sentinel(Sokoban_Change *change) {
    // TODO(diego): Clear sentinel memory
}

internal void
unlink(Sokoban_Change *change) {
    change->prev->next = change->next;
    change->next->prev = change->prev;
    
    change->prev = change;
    change->next = change;
}

internal Sokoban_Change *
pop_first(Sokoban_Change *prev) {
    Sokoban_Change *result = 0;
    if (prev->next != prev) {
        result = prev->next;
        unlink(prev->next);
    }
    return result;
}

internal void
push_first(Sokoban_Change *prev, Sokoban_Change *pushed) {
    link(prev, pushed);
}

internal void
undo(Sokoban_State *state) {
    real32 now = get_time();
    if (now - state->last_undo_time < SOKOBAN_UNDO_REDO_TIME) {
        return;
    }
    
    Sokoban_Change *change = pop_first(&state->undo_sentinel);
    assert(change);
    apply_change(state, change, SOKOBAN_CHANGE_FROM);
    push_first(&state->redo_sentinel, change);
    
    state->last_redo_time = 0.f;
    state->last_undo_time = get_time();
}

internal void
redo(Sokoban_State *state) {
    real32 now = get_time();
    if (now - state->last_redo_time < SOKOBAN_UNDO_REDO_TIME) {
        return;
    }
    
    Sokoban_Change *change = pop_first(&state->redo_sentinel);
    assert(change);
    apply_change(state, change, SOKOBAN_CHANGE_TO);
    push_first(&state->undo_sentinel, change);
    
    state->last_undo_time = 0.f;
    state->last_redo_time = get_time();
}

internal void
apply_change(Sokoban_State *state, Sokoban_Change *change, int type) {
    switch (change->type) {
        case SokobanChange_entity_location: {
            Sokoban_Entity_Location_Change *entity_location = &change->entity_location;
            if (type == SOKOBAN_CHANGE_FROM) {
                change_entity_location(state->world, entity_location->entity_index, &entity_location->change_to_value, &entity_location->change_from_value);
            } else {
                change_entity_location(state->world, entity_location->entity_index, &entity_location->change_from_value, &entity_location->change_to_value);
            }
        } break;
        
        case SokobanChange_entity_push: {
            Sokoban_Entity_Push_Change *entity_push = &change->entity_push;
            if (type == SOKOBAN_CHANGE_FROM) {
                change_entity_location(state->world, entity_push->pusher_index, &entity_push->pusher_change_to_value, &entity_push->pusher_change_from_value);
                change_entity_location(state->world, entity_push->pushed_index, &entity_push->pushed_change_to_value, &entity_push->pushed_change_from_value);
            } else {
                change_entity_location(state->world, entity_push->pusher_index, &entity_push->pusher_change_to_value, &entity_push->pusher_change_from_value);
                change_entity_location(state->world, entity_push->pushed_index, &entity_push->pushed_change_to_value, &entity_push->pushed_change_from_value);
            }
        } break;
        
        default: invalid_code_path; break;
    }
}

#define ALLOCATE_SOKOBAN_CHANGE(state, type) (&allocate_sokoban_change(state, SokobanChange_##type)->type)

internal Sokoban_Change *
allocate_sokoban_change(Sokoban_State *state, Sokoban_Change_Type change_type)
{
    Sokoban_Change *result = (Sokoban_Change *) platform_alloc(sizeof(Sokoban_Change));
    result->type = change_type;
    push_first(&state->undo_sentinel, result);
    return result;
}

internal void
recorded_change_entity_location(Sokoban_World *world, s32 entity_index, Sokoban_World_Position *old_position, Sokoban_World_Position *new_position) {
    Sokoban_Entity_Location_Change *change = ALLOCATE_SOKOBAN_CHANGE(world->state, entity_location);
    change->entity_index = entity_index;
    
    Sokoban_Entity *entity = &world->entities[entity_index];
    change->change_from_value = entity->world_position;
    
    change_entity_location(world, entity_index, old_position, new_position);
    
    change->change_to_value = entity->world_position;
}

internal void
change_entity_location(Sokoban_World *world, s32 entity_index, Sokoban_World_Position *old_position, Sokoban_World_Position *new_position) {
    if (old_position && are_same_world_position(old_position, new_position)) {
        // Do nothing
    } else {
        if (old_position) {
            ensure_world_position_is_valid(world, new_position);
            
            Sokoban_Entity *entity = &world->entities[entity_index];
            entity->world_position = *new_position;
            entity->position = to_visual_position(world, entity->world_position, entity->kind);
            
            // Active or deactivate goal if we changed a star
            if (entity->kind == SokobanEntityKind_Star) {
                s32 goal_activated = entity->entity_activated_index;
                if (goal_activated >= 0 && goal_activated < (s32) world->num_entities) {
                    Sokoban_Entity *activated_goal = &world->entities[goal_activated];
                    set_activate_goal_state(activated_goal, -1);
                    entity->entity_activated_index = -1;
                }
            }
        } else {
            // Entity is not in the world.
        }
    }
}

internal void
update_game(Sokoban_State *state, Game_Input *input) {
    
    Camera *cam = &state->cam;
    
    if (input->alt_is_down) {
        if (pressed(Button_Left)) {
            previous_level(state);
        }
        if (pressed(Button_Right)) {
            next_level(state);
        }
    } else {
        update_camera(cam, input);
    }
    
    if (cam->mode == CameraMode_LookAt) {
        
        Vector3 pos = cam->position;
        Vector3 t_pos = state->lock_position;
        
        real32 anim_amount = core.time_info.dt * state->cam_animation_rate*state->cam_animation_rate;
        cam->position.x = move_towards(cam->position.x, t_pos.x, anim_amount);
        cam->position.y = move_towards(cam->position.y, t_pos.y, anim_amount);
        cam->position.z = move_towards(cam->position.z, t_pos.z, anim_amount);
        cam->pitch = move_towards(cam->pitch, state->lock_pitch, anim_amount * 10.f);
        cam->yaw   = move_towards(cam->yaw,   state->lock_yaw,   anim_amount * 10.f);
        update_vectors(cam);
        
        
        Sokoban_World *current_world = state->world;
        Sokoban_Entity *player = &current_world->entities[state->player.entity_index];
        
        if (player) {
            
            Sokoban_World_Position old_player_position = player->world_position;
            Sokoban_World_Position new_player_position = player->world_position;
            if (pressed(Button_W)) {
                new_player_position.y -= 1;
            }
            if (pressed(Button_S)) {
                new_player_position.y += 1;
            }
            if (pressed(Button_A)) {
                new_player_position.x -= 1;
            }
            if (pressed(Button_D)) {
                new_player_position.x += 1;
            }
            
            if (!are_same_world_position(&old_player_position, &new_player_position)) {
                b32 allow_move = true;
                for (u32 entity_index = 0; entity_index < current_world->num_entities; ++entity_index) {
                    Sokoban_Entity *entity = &current_world->entities[entity_index];
                    if (player->id == entity->id) continue;
                    
                    if (are_same_world_position(&entity->world_position, &new_player_position)) {
                        if (entity->kind == SokobanEntityKind_Block) {
                            allow_move = false;
                            break;
                        } else if (is_pushable(entity->kind)) {
                            Sokoban_World_Position diff = {};
                            diff.x = new_player_position.x - old_player_position.x;
                            diff.y = new_player_position.y - old_player_position.y;
                            if (recorded_push_entity(state, player->id, entity_index, diff)) {
                                allow_move = false;
                                break;
                            }
                        }
                    }
                }
                if (allow_move) {
                    recorded_change_entity_location(current_world, player->id, &old_player_position, &new_player_position);
                }
            }
        }
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
    
    if (released(Button_Mouse1)) {
        place_entity(state->world, SokobanEntityKind_Star, intersect_position);
    }
    
    if (is_down(Button_Control)) {
        if (pressed(Button_Z) || is_down(Button_Z)) {
            if (undo_available(state)) undo(state);
        }
        if (pressed(Button_Y) || is_down(Button_Y)) {
            if (redo_available(state)) redo(state);
        }
    }
    
}

internal s32
is_position_occupied(Sokoban_World *world, Sokoban_World_Position test_position) {
    s32 result = -1;
    for (u32 entity_index = 0; entity_index < world->num_entities; ++entity_index) {
        Sokoban_Entity *entity = &world->entities[entity_index];
        b32 same_position = are_same_world_position(&test_position, &entity->world_position);
        if (same_position) {
            switch (entity->kind) {
                case SokobanEntityKind_Block:
                case SokobanEntityKind_Star:
                case SokobanEntityKind_Player:
                case SokobanEntityKind_Goal: {
                    result = entity_index;
                } break;
                default: break;
            }
            if (result > -1) break;
        }
    }
    return result;
}

internal b32
is_pushable(Sokoban_Entity_Kind kind) {
    return kind == SokobanEntityKind_Star;
}

internal b32
recorded_push_entity(Sokoban_State *state, s32 pusher_index, s32 pushed_index, Sokoban_World_Position diff) {
    
    Sokoban_Entity *pusher_entity = &state->world->entities[pusher_index];
    Sokoban_Entity *pushed_entity = &state->world->entities[pushed_index];
    
    Sokoban_World_Position pusher_old_position = pusher_entity->world_position;
    Sokoban_World_Position pushed_old_position = pushed_entity->world_position;
    
    b32 result = push_entity(state->world, pusher_index, pushed_index, diff);
    if (result) {
        Sokoban_Entity_Push_Change *change = ALLOCATE_SOKOBAN_CHANGE(state, entity_push);
        change->pusher_index = pusher_index;
        change->pushed_index = pushed_index;
        
        change->pusher_change_from_value = pusher_old_position;
        change->pushed_change_from_value = pushed_old_position;
        
        change->pusher_change_to_value = pusher_entity->world_position;
        change->pushed_change_to_value = pushed_entity->world_position;
    }
    
    return result;
}

internal b32
push_entity(Sokoban_World *world, s32 pusher_index, s32 pushed_index, Sokoban_World_Position diff) {
    assert(pusher_index >= 0 && (u32) pusher_index < world->num_entities);
    assert(pushed_index >= 0 && (u32) pushed_index < world->num_entities);
    if (diff.x == 0 && diff.y == 0) return false;
    
    Sokoban_Entity *pusher = &world->entities[pusher_index];
    Sokoban_Entity *pushed = &world->entities[pushed_index];
    
    // Calculate entity to be pushed final position
    Sokoban_World_Position final_position = {};
    final_position.x = pushed->world_position.x + diff.x;
    final_position.y = pushed->world_position.y + diff.y;
    
    Sokoban_Entity *occupied = 0;
    
    s32 occupied_index = is_position_occupied(world, final_position);
    if (occupied_index >= 0) {
        occupied = &world->entities[occupied_index];
        if (occupied->kind != SokobanEntityKind_Goal || occupied->entity_activated_index >= 0)
            return true;
    }
    
    // If we can push it then change both entities position
    change_entity_location(world, pusher_index, &pusher->world_position, &pushed->world_position);
    change_entity_location(world, pushed_index, &pushed->world_position, &final_position);
    
    // Activate Goal if necessary
    if (occupied && occupied->kind == SokobanEntityKind_Goal) {
        pushed->entity_activated_index = occupied_index;
        set_activate_goal_state(occupied, pushed_index);
    }
    
    return true;
}

internal void
set_activate_goal_state(Sokoban_Entity *entity, s32 activator_index) {
    assert(entity->kind == SokobanEntityKind_Goal);
    
    entity->entity_activated_index = activator_index;
    if (activator_index == -1) {
        entity->color = make_color(0xffffffff);
    } else {
        entity->color = make_color(0xffff00ff);
    }
}

internal void
release_current_level(Sokoban_State *state) {
    if (state->world->num_entities > 0 && state->world->entities) {
        platform_free(state->world->entities);
        platform_free(state->world);
    }
}

internal void
previous_level(Sokoban_State *state) {
    if (state->current_level - 1 < 0) return;
    
    char lvlname[256];
    sprintf(lvlname, "sasquatch_v_%d", state->current_level - 1);
    
    Sokoban_World *level = load_level(state, lvlname);
    if (!level) {
        return;
    }
    release_current_level(state);
    state->world = level;
    state->current_level--;
    adjust_camera_to_level(state, false);
}

internal void
next_level(Sokoban_State *state) {
    char lvlname[256];
    sprintf(lvlname, "sasquatch_v_%d", state->current_level + 1);
    
    Sokoban_World *level = load_level(state, lvlname);
    if (!level) {
        return;
    }
    release_current_level(state);
    state->world = level;
    state->current_level++;
    adjust_camera_to_level(state, false);
}

internal Sokoban_World *
load_level(Sokoban_State *state, char *levelname) {
    
    Sokoban_World *result = 0;
    
    char *folder = "./data/levels/sokoban/";
    char *lvl = ".lvl";
    
    char *filename = concat(levelname, lvl, string_length(levelname), string_length(lvl));
    char *filepath = concat(folder, filename, string_length(folder), string_length(filename));
    
    platform_free(filename);
    
    File_Contents level = platform_read_entire_file(filepath);
    if (level.file_size == 0) {
        return result;
    }
    
    platform_free(filepath);
    
    result = (Sokoban_World *) platform_alloc(sizeof(Sokoban_World));
    
    u8 *at = level.contents;
    
    u32 x_count = 0;
    while (*at) {
        if (*at == '\r') {
            // No-op
        }
        else if (*at == '\n') {
            result->y_count++;
            if (x_count > result->x_count) {
                result->x_count = x_count;
            }
            x_count = 0;
        }
        // We say empty spaces are entities for now.
        else {
            x_count++;
        }
        at++;
    }
    
    result->state = state;
    result->num_entities += result->x_count*result->y_count;
    assert(result->num_entities > 0);
    
    result->entities = (Sokoban_Entity *) platform_alloc(result->num_entities);
    
    at = level.contents;
    
    u32 entity_id = 0;
    
    u32 xx = 0;
    u32 yy = 0;
    while (*at) {
        if (*at == '\r') {
            // No-op
        }
        else if (*at == '\n') {
            yy++;
            if (xx >= result->x_count) {
                xx = 0;
            }
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
                entity.id = entity_id;
                entity.position = to_visual_position(result, entity.world_position, kind);
                
                Sokoban_Entity *entity_slot = &result->entities[entity_id];
                
                // If player set the player
                if (kind == SokobanEntityKind_Player) {
                    entity.color = make_color(0xffff00ff);
                    
                    Sokoban_Player player = {};
                    player.entity_index = entity_id;
                    state->player = player;
                }
                *entity_slot = entity;
                entity_id++;
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
        
        for (u32 i = 0; i < state->world->num_entities; ++i) {
            Sokoban_Entity entity = state->world->entities[i];
            if (entity.kind == SokobanEntityKind_None) continue;
            
            Vector4 *override_color = 0;
            Triangle_Mesh *mesh = 0;
            
            Quaternion orientation = quaternion_identity();
            real32 scale = 1.f;
            
            switch (entity.kind) {
                case SokobanEntityKind_Block: {
                    mesh = &state->block; 
                } break;
                case SokobanEntityKind_Goal: {
                    mesh = &state->goal;
                    override_color = &entity.color;
                } break;
                case SokobanEntityKind_Star: {
                    orientation = make_quaternion(make_vector3(0.f, 1.f, .0f), angle);
                    entity.position.y += sinf(core.time_info.current_time * 2.f) * .02f;
                    mesh = &state->star;
                } break; 
                case SokobanEntityKind_Sun: {
                    orientation = make_quaternion(make_vector3(0.f, 1.f, .0f), sun_angle);
                    mesh = &state->sun;
                } break;
                case SokobanEntityKind_Player: {
                    override_color = &entity.color;
                    scale = 0.5f;
                    mesh = &state->block;
                } break;
                
                default: break;
            }
            
            if (mesh) {
                draw_mesh(mesh, entity.position, orientation, scale, override_color);
            }
        }
    }
    
    //
    // Draw placed entity
    //
    if (placed_entity) {
        draw_mesh(&state->block, placed_entity->position, a);
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
    
    u32 xc = state->world->x_count;
    u32 yc = state->world->y_count;
    
    int max_count = (int) (xc > yc ? xc : yc);
    max_count *= 2;
    
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
        
        Vector2i dim = state->dimensions;
        
        int width = dim.width;
        int height = dim.height;
        
        ensure_framebuffer(width, height);
        use_framebuffer(&immediate->multisampled_framebuffer);
        
        render_3d(width, height, state->cam.fov);
        glClearColor(0.f/255.f, 170.f/255.f, 255.f/255.f, 255.f/255.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //
        // Draw grid
        //
        
        view_matrix = get_view_matrix(&state->cam);
        
        mouse_ray = ray_from_mouse(state->dimensions);
        
        set_shader(global_basic_3d_shader);
        refresh_shader_transform();
        
        set_mat4("light_space_matrix", light_space_matrix);
        set_float3("light_pos", light_pos);
        set_float3("view_position", state->cam.position);
        set_texture("shadow_map", &immediate->depth_map);
        
        draw_game_playing(state);
        
        // Ray test
        {
            Vector3 n = make_vector3(0.f, 1.f, 0.f);
            Vector3 d = mouse_ray * 100.f;
            
            Vector3 start = state->cam.position;
            Vector3 end = state->cam.position + d;
            
            b32 draw_intersection = false;
            if (trace_line(state->world, start, end, intersect_position)) {
                draw_intersection = true;
            } else {
                real32 t;
                if (line_plane_intersection(n, origin, start, end, intersect_position, t)) {
                    draw_intersection = true;
                }
            }
            if (draw_intersection) {
                snap(&intersect_position, 0.25f);
                Vector3 position = intersect_position;
                draw_mesh(&state->block, position, quaternion_identity(), 1.f);
            }
        }
        
        draw_grid(state);
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
        
        state->block = load_mesh("./data/models/sokoban/block.obj", MESH_FLIP_UVS);
        state->star  = load_mesh("./data/models/sokoban/star.obj", MESH_FLIP_UVS);
        state->plane = load_mesh("./data/models/sokoban/plane.obj", MESH_FLIP_UVS);
        state->sun   = load_mesh("./data/models/sokoban/sun.obj", MESH_FLIP_UVS);
        state->arrow = load_mesh("./data/models/sokoban/arrow.obj", MESH_FLIP_UVS);
        state->goal  = load_mesh("./data/models/sokoban/goal.obj", MESH_FLIP_UVS);
        
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