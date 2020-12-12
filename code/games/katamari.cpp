internal katamari_entity
make_entity(katamari_entity_kind kind) {
    
    katamari_entity result = {};
    result.kind = kind;
    result.alive = true;
    
    switch (kind) {
        case KatamariEntity_Player: {
            result.half_size = make_v2(KATAMARI_PLAYER_SIZE/2.f, KATAMARI_PLAYER_SIZE/2.f);
        } break;
        
        case KatamariEntity_Squirrel1: {
            result.half_size = make_v2(KATAMARI_SQUIRREL_MAX_SIZE / 3.f, KATAMARI_SQUIRREL_MAX_SIZE / 3.f);
        } break;
        
        case KatamariEntity_Squirrel2: {
            result.half_size = make_v2(KATAMARI_SQUIRREL_MAX_SIZE / 2.f, KATAMARI_SQUIRREL_MAX_SIZE / 2.f);
        } break;
        
        case KatamariEntity_Squirrel3: {
            result.half_size = make_v2(KATAMARI_SQUIRREL_MAX_SIZE, KATAMARI_SQUIRREL_MAX_SIZE);
        } break;
        
        case KatamariEntity_Grass1: {
            result.half_size = make_v2(60.f / 2.f, 24.f / 2.f);
        } break;
        
        case KatamariEntity_Grass2:
        case KatamariEntity_Grass3: {
            result.half_size = make_v2(52.f / 2.f, 48.f / 2.f);
        } break;
        
        default: invalid_code_path;
    }
    
    return result;
}

internal katamari_entity
make_grass(katamari_entity_kind kind, real32 x, real32 y) {
    assert(kind == KatamariEntity_Grass1 || kind == KatamariEntity_Grass2 || kind == KatamariEntity_Grass3);
    
    katamari_entity result = make_entity(kind);
    
    result.position.x = x;
    result.position.y = y;
    
    return result;
}

internal katamari_entity*
find_first_entity(katamari_state *state, katamari_entity_kind kind) {
    for (int entity_index = 0; entity_index < array_count(state->entities); ++entity_index) {
        katamari_entity *entity = &state->entities[entity_index];
        if (entity->kind == kind) {
            return entity;
        }
    }
    return 0;
}

internal void
spawn_squirrel(katamari_state *state, u32 count) {
    
    v2 center = make_v2(state->dimensions.width * .5f, state->dimensions.height * .5f);
    
    u32 past_squirrel_index = KatamariEntity_Squirrel3 + 1;
    for (u32 i = 0; i < count; ++i) {
        // Get first valid position for the new squirrel.
        u32 squirrel_index = 0;
        for (int ii = KATAMARI_SQUIRREL_BASE_INDEX; ii < array_count(state->entities); ++ii) {
            if (!state->entities[ii].alive) {
                squirrel_index = ii;
                break;
            }
        }
        if (!squirrel_index) {
            // If we don't have one valid position then we can't spawn any squirrel.
            break;
        }
        
        assert(squirrel_index >= KATAMARI_SQUIRREL_BASE_INDEX && squirrel_index < array_count(state->entities));
        katamari_entity_kind kind = (katamari_entity_kind) (random_u32() % past_squirrel_index);
        if (kind < KatamariEntity_Squirrel1) {
            kind = (katamari_entity_kind) (kind + KatamariEntity_Squirrel1);
        }
        assert(kind >= KatamariEntity_Squirrel1 && kind <= KatamariEntity_Squirrel3);
        
        katamari_entity new_squirrel = make_entity(kind);
        
        new_squirrel.position.x = random_real32_in_range(-center.x, center.x);
        new_squirrel.position.y = random_real32_in_range(-center.y, center.y);
        new_squirrel.speed = KATAMARI_SQUIRREL_SPEED;
        new_squirrel.stopped_t_target = .0f;
        new_squirrel.stopped_t = .0f;
        
#if 1
        real32 r = random_real32_in_range(0.f, 1.f);
        if (r > .75f) {
            new_squirrel.movement_pattern = KatamariMovementPattern_ZigZag;
            new_squirrel.zigzag_t = 0.f;
            new_squirrel.zigzag_target = new_squirrel.speed / random_real32_in_range(1.f, 20.f);
        } else if (r > .5f) {
            new_squirrel.movement_pattern = KatamariMovementPattern_LeftRight;
        } else if (r > .25f) {
            new_squirrel.movement_pattern = KatamariMovementPattern_TopBottom;
        } else {
            new_squirrel.movement_pattern = KatamariMovementPattern_FollowPlayer;
        }
#else
        new_squirrel.movement_pattern = KatamariMovementPattern_ZigZag;
#endif
        
        // NOTE(diego): We don't care how this is computed.
        int x_direction = random_int_in_range(-1, 1);
        while (x_direction == 0) {
            x_direction = random_int_in_range(-1, 1);
        }
        int y_direction = random_int_in_range(-1, 1);
        while (y_direction == 0) {
            y_direction = random_int_in_range(-1, 1);
        }
        new_squirrel.direction  = make_v2((real32) x_direction,
                                          (real32) y_direction);
        state->entities[squirrel_index] = new_squirrel;
    }
}

internal b32
squirrel_collided(katamari_state *state, katamari_entity *squirrel, u8 wall_number) {
    
    v2 center = make_v2(state->dimensions.width * .5f, state->dimensions.height * .5f);
    
    b32 result = false;
    
    switch (wall_number) {
        case 0:
        case 1: { // left or right
            result = squirrel->position.x < -center.x || squirrel->position.x > center.x; 
        } break;
        
        case 2:
        case 3: { // top or bottom
            result = squirrel->position.y < -center.y || squirrel->position.y > center.y;
        } break;
        
        default: {
            // No-op
        } break;
    }
    
    return result;
}

#define DEBUG_BOX 0

#if DEBUG_BOX
internal void
draw_box_outline(katamari_state *state, box b) {
    
    v2 center = make_v2(state->dimensions.width * .5f, state->dimensions.height * .5f);
    
    real32 stwidth = 2.f;
    
    v2 tmin = make_v2(center.x + b.x, center.y + b.y);
    v2 tmax = make_v2(tmin.x + b.width, tmin.y + stwidth);
    
    v2 bmin = make_v2(center.x + b.x, center.y + b.y + b.height);
    v2 bmax = make_v2(bmin.x + b.width, bmin.y - stwidth);
    
    v4 color = make_color(0xFFFFFF00);
    
    real32 z_index = -5.f;
    
    immediate_begin();
    immediate_quad(tmin, tmax, color, z_index);
    immediate_quad(bmin, bmax, color, z_index);
    immediate_flush();
}
#endif

internal void
squirrel_handle_collision(katamari_state *state, katamari_entity *player, katamari_entity *squirrel) {
    
    box sbox = {};
    sbox.x = squirrel->position.x - squirrel->half_size.x;
    sbox.y = squirrel->position.y - squirrel->half_size.y;
    sbox.width = squirrel->half_size.x; 
    sbox.height = squirrel->half_size.y;
    
    box pbox = {};
    pbox.x = player->position.x - player->half_size.x;
    pbox.y = player->position.y - player->half_size.y;
    pbox.width = player->half_size.x; 
    pbox.height = player->half_size.y;
    
#if DEBUG_BOX
    draw_box_outline(state, sbox);
    draw_box_outline(state, pbox);
#endif
    
    if (aabb_vs_aabb(pbox, sbox)) {
        real32 ls = length(squirrel->half_size);
        real32 lp = length(player->half_size);
        
        if (ls > lp) {
            state->health--;
            if (state->health < 1) {
                state->game_mode = GameMode_GameOver;
                return;
            }
        } else {
            player->half_size = player->half_size + squirrel->half_size;
            squirrel->alive = false;
            if (state->health < 3) {
                state->health++;
            }
        }
        return;
    }
    
    b32 lr_collided = squirrel_collided(state, squirrel, 0) || squirrel_collided(state, squirrel, 1);
    b32 tb_collided = squirrel_collided(state, squirrel, 2) || squirrel_collided(state, squirrel, 3);
    
    if (squirrel->movement_pattern == KatamariMovementPattern_LeftRight) {
        if (lr_collided) 
            squirrel->direction.x = -squirrel->direction.x;
    }
    if (squirrel->movement_pattern == KatamariMovementPattern_TopBottom) {
        if (tb_collided)
            squirrel->direction.y = -squirrel->direction.y;
    }
    if (squirrel->movement_pattern == KatamariMovementPattern_ZigZag) {
        if (lr_collided) {
            squirrel->direction.x = -squirrel->direction.x;
            squirrel->zigzag_t = .0f;
        }
        if (tb_collided) {
            squirrel->direction.y = -squirrel->direction.y;
            squirrel->zigzag_t = .0f;
        }
    }
}

internal void
init_game(katamari_state *state) {
    state->game_mode = GameMode_Playing;
    state->health = 3;
    
    katamari_entity player = make_entity(KatamariEntity_Player);
    
    init_textures(&state->assets);
    
    // Add grasses later or never :P
    int entity_index = 0;
    state->entities[entity_index++] = make_grass(KatamariEntity_Grass1, -100.f, -100.f);
    state->entities[entity_index++] = make_grass(KatamariEntity_Grass1, 100.f, -100.f);
    state->entities[entity_index++] = make_grass(KatamariEntity_Grass1, -300.f, 300.f);
    
    state->entities[entity_index++] = make_grass(KatamariEntity_Grass2, -200.f, 100.f);
    state->entities[entity_index++] = make_grass(KatamariEntity_Grass2, -210.f, 300.f);
    state->entities[entity_index++] = make_grass(KatamariEntity_Grass2, -120.f, 450.f);
    
    state->entities[entity_index++] = make_grass(KatamariEntity_Grass3, -270.f, 30.f);
    state->entities[entity_index++] = make_grass(KatamariEntity_Grass3, -20.f, -20.f);
    state->entities[entity_index++] = make_grass(KatamariEntity_Grass3, 360.f, 50.f);
    
    // Last to be drawn
    state->entities[entity_index++] = player;
    
    state->spawn_t = 0.f;
    state->spawn_t_target = 1.f;
}

internal void
init_textures(katamari_assets *assets) {
    assets->grass[0] = load_texture("./data/textures/katamari/grass_01.png");
    assets->grass[1] = load_texture("./data/textures/katamari/grass_02.png");
    assets->grass[2] = load_texture("./data/textures/katamari/grass_03.png");
    assets->squirrel[0] = load_texture("./data/textures/katamari/squirrel_r.png");
    assets->squirrel[1] = load_texture("./data/textures/katamari/squirrel_l.png");
}

internal void
free_textures(katamari_assets *assets) {
    assert(assets);
    
    glDeleteTextures(array_count(assets->grass), assets->grass);
    glDeleteTextures(array_count(assets->squirrel), assets->squirrel);
}

internal void
update_game(katamari_state *state, game_input *input) {
    
    katamari_entity *player = find_first_entity(state, KatamariEntity_Player);
    assert(player);
    
    v2 acceleration = {};
    
    if (is_down(Button_Left)) {
        acceleration.x = -1.f;
    }
    if (is_down(Button_Right)) {
        acceleration.x = 1.f;
    }
    if (is_down(Button_Up)) {
        acceleration.y = -1.f;
    }
    if (is_down(Button_Down)) {
        acceleration.y = 1.f;
    }
    
    real32 speed = 200.f;
    acceleration *= speed;
    acceleration += -2.f * player->velocity;
    
    // NOTE(diego): Equation of Motions!
    real32 square_dt = time_info.dt * time_info.dt;
    player->position = (0.5f*acceleration*square_dt +
                        player->velocity*time_info.dt + player->position);
    player->velocity = acceleration*time_info.dt + player->velocity;
    
    //
    // Update squirrels
    //
    
    for (u32 squirrel_index = KATAMARI_SQUIRREL_BASE_INDEX; squirrel_index < array_count(state->entities); ++squirrel_index) {
        katamari_entity *squirrel = &state->entities[squirrel_index];
        if (!squirrel->alive) continue;
        
        v2 vel = {};
        if (squirrel->movement_pattern == KatamariMovementPattern_LeftRight) {
            vel.x += squirrel->direction.x * squirrel->speed * time_info.dt;
        } else if (squirrel->movement_pattern == KatamariMovementPattern_TopBottom) {
            vel.y += squirrel->direction.y * squirrel->speed * time_info.dt;
        } else if (squirrel->movement_pattern == KatamariMovementPattern_FollowPlayer) {
            v2 distance = player->position - squirrel->position;
            if (length(distance) < 50.f) {
                v2 normalized = normalize(distance);
                vel.x += normalized.x * squirrel->speed * time_info.dt;
                vel.y += normalized.y * squirrel->speed * time_info.dt;
            }
        } else if (squirrel->movement_pattern == KatamariMovementPattern_ZigZag) {
            if (squirrel->stopped_t_target > .0f) {
                squirrel->stopped_t += time_info.dt;
                if (squirrel->stopped_t > squirrel->stopped_t_target) {
                    squirrel->stopped_t = .0f;
                    squirrel->stopped_t_target = .0f;
                }
            } else {
                if (squirrel->zigzag_t > squirrel->zigzag_target) {
                    squirrel->zigzag_t -= squirrel->zigzag_target;
                    squirrel->stopped_t_target = random_real32_in_range(1.f, 3.f);
                    real32 r = random_real32_in_range(0.f, 1.f);
                    if (r > 0.5f) {
                        squirrel->direction.x = -squirrel->direction.x;
                    } else {
                        squirrel->direction.y = -squirrel->direction.y;
                    }
                }
                
                vel.x += squirrel->direction.x * squirrel->speed * time_info.dt;
                vel.y += squirrel->direction.y * squirrel->speed * time_info.dt;
                squirrel->zigzag_t += time_info.dt;
            }
        }
        
        squirrel->position = squirrel->position + vel;
        squirrel_handle_collision(state, player, squirrel);
    }
    
    if (player->half_size.x > state->dimensions.width &&
        player->half_size.y > state->dimensions.height) {
        state->game_mode = GameMode_GameOver;
    }
    
    if (state->spawn_t >= state->spawn_t_target) {
        state->spawn_t -= state->spawn_t_target;
        spawn_squirrel(state, 1);
    }
    
    state->spawn_t += time_info.dt;
}

internal void
draw_player(katamari_state *state, katamari_entity *entity) {
    
    v2 center = make_v2(state->dimensions.width * .5f, state->dimensions.height * .5f);
    v2 min = make_v2(center.x + entity->position.x - entity->half_size.x,
                     center.y + entity->position.y - entity->half_size.y);
    v2 max = make_v2(min.x + entity->half_size.x, min.y + entity->half_size.y);
    
    v4 color = make_color(0xFFFFFFFF);
    
    immediate_begin();
    immediate_quad(min, max, color);
    immediate_flush();
}

internal void
draw_squirrel(katamari_state *state, katamari_entity *entity) {
    v2 center = make_v2(state->dimensions.width * .5f, state->dimensions.height * .5f);
    v2 min = make_v2(center.x + entity->position.x - entity->half_size.x,
                     center.y + entity->position.y - entity->half_size.y);
    v2 max = make_v2(min.x + entity->half_size.x, min.y + entity->half_size.y);
    
    GLuint texture = state->assets.squirrel[0];
    if (entity->direction.x < 0) {
        texture = state->assets.squirrel[1];
    }
    
    // NOTE(diego): To draw all squirrel with a single draw call we could sort the entities array and use onde immediate_begin and immediate_flush for each group of entities.
    // Or save all squirrels in an exclusive array.
    immediate_begin();
    immediate_textured_quad(min, max, texture);
    immediate_flush();
}

internal void
draw_grass(katamari_state *state, katamari_entity *entity) {
    v2 center = make_v2(state->dimensions.width * .5f, state->dimensions.height * .5f);
    v2 min = make_v2(center.x + entity->position.x - entity->half_size.x,
                     center.y + entity->position.y - entity->half_size.y);
    v2 max = make_v2(min.x + entity->half_size.x, min.y + entity->half_size.y);
    
    int index = (int) (entity->kind - KatamariEntity_Grass1);
    assert(index >= 0 && index < array_count(state->assets.grass));
    
    // NOTE(diego): To draw all grasses with a single draw call we could sort the entities array and use one immediate_begin and immediate_flush for each group of entities.
    // Or save all grasses in an exclusive array.
    immediate_begin();
    immediate_textured_quad(min, max, state->assets.grass[index]);
    immediate_flush();
}

internal void
draw_entity(katamari_state *state, katamari_entity *entity) {
    if (!entity) return;
    if (!entity->alive) return;
    
    switch (entity->kind) {
        case KatamariEntity_Player: draw_player(state, entity); break;
        case KatamariEntity_Squirrel1:
        case KatamariEntity_Squirrel2:
        case KatamariEntity_Squirrel3: {
            draw_squirrel(state, entity); 
        } break;
        case KatamariEntity_Grass1:
        case KatamariEntity_Grass2:
        case KatamariEntity_Grass3: {
            draw_grass(state, entity);
        } break;
        default: invalid_code_path;
    }
}

internal void
draw_hud(katamari_state *state) {
    immediate_begin();
    
    real32 health_x_margin = 20.f;
    real32 health_y_margin = 20.f;
    
    real32 health_pad = 4.f;
    
    real32 health_width = 10.f;
    real32 health_height = 10.f;
    
    v4 color = make_color(0xFFFF0000);
    v4 backing_color = make_color(0xFFFFFFFF);
    
    v2 backing_min = make_v2(health_x_margin - 2.f, health_y_margin);
    v2 backing_max = make_v2(backing_min.x + health_width + health_pad, backing_min.y + state->health * health_height + health_pad * state->health);
    
    immediate_quad(backing_min, backing_max, backing_color);
    
    real32 y_cursor = health_y_margin + health_pad * .5f;
    
    for (u8 h = 0; h < state->health; ++h) {
        v2 min = make_v2(health_x_margin, y_cursor);
        v2 max = make_v2(min.x + health_width, min.y + health_height);
        immediate_quad(min, max, color);
        
        y_cursor += health_pad + health_height;
    }
    
    immediate_flush();
}

internal void
draw_game_view(katamari_state *state) {
    if (state->game_mode == GameMode_Playing) {
        immediate_begin();
        immediate_quad(0.f, 0.f, (real32) state->dimensions.width, (real32) state->dimensions.height, make_color(0xff14ce00));
        immediate_flush();
        
        for (int entity_index = 0; entity_index < array_count(state->entities); ++entity_index) {
            katamari_entity *entity = &state->entities[entity_index];
            if (entity->kind != KatamariEntity_None) {
                draw_entity(state, entity);
            }
        }
        
        draw_hud(state);
    } else {
        draw_menu(KATAMARI_DAMACY_TITLE, state->dimensions, state->game_mode, state->menu_selected_item, state->quit_was_selected);
    }
}

internal void
katamari_menu_art(app_state *state, v2 min, v2 max) {
    immediate_begin();
    immediate_textured_quad(min, max, state->menu_art.katamari);
    immediate_flush();
}

internal void
katamari_game_restart(katamari_state *state) {
    init_game(state);
}

internal void
katamari_game_update_and_render(game_memory *memory, game_input *input) {
    
    katamari_state *state = (katamari_state *) memory->permanent_storage;
    if (!memory->initialized) {
        assert(memory->permanent_storage_size == 0);
        assert(!memory->permanent_storage);
        memory->initialized = true;
        
        state = (katamari_state *) game_alloc(memory, megabytes(12));
        
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
    } else if (state->game_mode == GameMode_Menu || state->game_mode == GameMode_GameOver) {
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
                    katamari_game_restart(state);
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
katamari_game_free(game_memory *memory) {
    if (!memory) return;
    
    katamari_state *state = (katamari_state *) memory->permanent_storage;
    if (!state) return;
    
    free_textures(&state->assets);
}