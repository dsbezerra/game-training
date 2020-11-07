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

internal void
squirrel_handle_collision(katamari_state *state, katamari_entity *player, katamari_entity *squirrel) {
    // TODO(diego): Handle player collision.
    v2 center = make_v2(state->dimensions.width * .5f, state->dimensions.height * .5f);
    if (squirrel->position.x < -center.x || squirrel->position.x > center.x)
        squirrel->direction.x = -squirrel->direction.x;
    if (squirrel->position.y < -center.y || squirrel->position.y > center.y)
        squirrel->direction.y = -squirrel->direction.y;
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
    
    // TODO(diego): Implement equation of motions
    real32 speed = 100.f * time_info.dt;
    v2 velocity = {};
    if (is_down(Button_Left)) {
        velocity.x -= speed;
    }
    if (is_down(Button_Right)) {
        velocity.x += speed;
    }
    if (is_down(Button_Up)) {
        velocity.y -= speed;
    }
    if (is_down(Button_Down)) {
        velocity.y += speed;
    }
    player->position = add_v2(player->position, velocity);
    
    //
    // Update squirrels
    //
    
    for (u32 squirrel_index = KATAMARI_SQUIRREL_BASE_INDEX; squirrel_index < array_count(state->entities); ++squirrel_index) {
        katamari_entity *squirrel = &state->entities[squirrel_index];
        if (!squirrel->alive) continue;
        
        v2 vel = {};
        vel.x += squirrel->direction.x * KATAMARI_SQUIRREL_SPEED * time_info.dt;
        vel.y += squirrel->direction.y * KATAMARI_SQUIRREL_SPEED * time_info.dt;
        squirrel->position = add_v2(squirrel->position, vel);
        squirrel_handle_collision(state, player, squirrel);
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
    immediate_quad(min, max, color, 1.f);
    immediate_flush();
}

internal void
draw_squirrel(katamari_state *state, katamari_entity *entity) {
    v2 center = make_v2(state->dimensions.width * .5f, state->dimensions.height * .5f);
    v2 min = make_v2(center.x + entity->position.x - entity->half_size.x,
                     center.y + entity->position.y - entity->half_size.y);
    v2 max = make_v2(min.x + entity->half_size.x, min.y + entity->half_size.y);
    
    // NOTE(diego): To draw all squirrel with a single draw call we could sort the entities array and use onde immediate_begin and immediate_flush for each group of entities.
    // Or save all squirrels in an exclusive array.
    immediate_begin();
    immediate_textured_quad(min, max, state->assets.squirrel[0], 1.f);
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
    immediate_textured_quad(min, max, state->assets.grass[index], 1.f);
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
    v2 backing_max = make_v2(backing_min.x + health_width + health_pad, backing_min.y + state->health * health_height + (state->health - 1) * health_pad);
    
    immediate_quad(backing_min, backing_max, backing_color, 1.f);
    
    real32 y_cursor = health_y_margin + health_pad * .5f;
    
    for (u8 h = 0; h < state->health; ++h) {
        v2 min = make_v2(health_x_margin, y_cursor);
        v2 max = make_v2(min.x + health_width, min.y + health_height);
        immediate_quad(min, max, color, 1.f);
        
        y_cursor += health_pad *.5f + health_height;
    }
    
    immediate_flush();
}

internal void
draw_game_view(katamari_state *state) {
    if (state->game_mode == GameMode_Playing) {
        immediate_begin();
        immediate_quad(0.f, 0.f, (real32) state->dimensions.width, (real32) state->dimensions.height, make_color(0xff14ce00), 1.f);
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
    v4 background = make_color(0xff5a1c5f);
    immediate_begin();
    immediate_quad(min, max, background, 1.f);
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