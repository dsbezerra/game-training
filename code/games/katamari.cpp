internal katamari_entity
make_entity(katamari_entity_kind kind) {
    
    katamari_entity result = {};
    result.kind = kind;
    
    switch (kind) {
        case KatamariEntity_Player: {
            result.half_size = make_v2(KATAMARI_PLAYER_SIZE/2.f, KATAMARI_PLAYER_SIZE/2.f);
        } break;
        
        case KatamariEntity_Squirrel: {
            // TODO(diego): Create squirrel.
        } break;
        
        default: invalid_code_path;
    }
    
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
init_game(katamari_state *state) {
    state->game_mode = GameMode_Playing;
    
    katamari_entity player = make_entity(KatamariEntity_Player);
    
    int entity_index = 0;
    state->entities[entity_index++] = player;
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
}

internal void
draw_player(katamari_state *state, katamari_entity *entity) {
    
    v2 center = make_v2(state->dimensions.width * .5f, state->dimensions.height * .5f);
    v2 min = make_v2(center.x + entity->position.x - entity->half_size.x,
                     center.y + entity->position.y - entity->half_size.y);
    v2 max = make_v2(min.x + entity->half_size.x, min.y + entity->half_size.y);
    
    v4 color = make_color(0xFFFFFFFF);
    
    immediate_quad(min, max, color, 1.f);
}

internal void
draw_squirrel(katamari_state *state, katamari_entity *entity) {
    v2 center = make_v2(state->dimensions.width * .5f, state->dimensions.height * .5f);
    v2 min = make_v2(center.x + entity->position.x - entity->half_size.x,
                     center.y + entity->position.y - entity->half_size.y);
    v2 max = make_v2(min.x + entity->half_size.x, min.y + entity->half_size.y);
    
    v4 color = make_color(0xFFCC8200);
    
    immediate_quad(min, max, color, 1.f);
}

internal void
draw_entity(katamari_state *state, katamari_entity *entity) {
    if (!entity) return;
    
    switch (entity->kind) {
        case KatamariEntity_Player: draw_player(state, entity); break;
        case KatamariEntity_Squirrel: draw_squirrel(state, entity); break;
        default: invalid_code_path;
    }
}

internal void
draw_game_view(katamari_state *state) {
    if (state->game_mode == GameMode_Playing) {
        immediate_begin();
        
        immediate_quad(0.f, 0.f, (real32) state->dimensions.width, (real32) state->dimensions.height, make_color(0xff14ce00), 1.f);
        
        for (int entity_index = 0; entity_index < array_count(state->entities); ++entity_index) {
            katamari_entity *entity = &state->entities[entity_index];
            if (entity->kind != KatamariEntity_None) {
                draw_entity(state, entity);
            }
        }
        immediate_flush();
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
}