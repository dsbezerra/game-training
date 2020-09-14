
internal void
dodger_game_restart(dodger_state *state) {
    state->game_mode = GameMode_Playing;
    state->score = 0;
    state->quit_was_selected = false;
    state->menu_selected_item = 0;
    
    //
    // Re-init
    //
    
    init_player(state);
    init_bad_guys(state);
}

internal void
init_player(dodger_state *state) {
    dodger_player player = {};
    
    v2i dim = state->world.dimensions;
    player.position = make_v2(dim.width / 2.f, dim.height * .2f);
    player.size = make_v2(25.f, 25.0f);
    
    state->player = player;
}

internal void
init_bad_guy(dodger_state *state, dodger_bad_guy *bad_guy) {
    v2i dim = state->world.dimensions;
    v2 position = make_v2(random_real32_in_range(0.f, dim.width * 1.f), random_real32_in_range((real32) -dim.height, -dim.height / 2.f));
    real32 wh = random_real32_in_range(20.f, 50.f);
    v2 size = make_v2(wh, wh);
    bad_guy->position = position;
    bad_guy->size = size;
}

internal void
init_bad_guys(dodger_state *state) {
    for (u32 bad_guy_index = 0; 
         bad_guy_index < array_count(state->bad_guys);
         ++bad_guy_index) {
        dodger_bad_guy *bad_guy = &state->bad_guys[bad_guy_index]; 
        init_bad_guy(state, bad_guy);
    }
}

internal void
update_player(dodger_state *state, game_input *input, real32 dt) {
    
    dodger_player *player = &state->player;
    
    v2i dim = state->world.dimensions;
    
    game_mouse mouse = input->mouse;
    v2 velocity = make_v2(mouse.sensitivity * mouse.velocity.x, mouse.sensitivity * mouse.velocity.y);
    
    real32 speed = 100.f * dt;
    if (is_down(Button_Space)) {
        speed *= 2;
    }
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
    
    v2 new_position = add_v2(player->position, velocity);
    new_position.x = clampf(0.f, new_position.x, ((real32) dim.width) - player->size.width);
    new_position.y = clampf(0.f, new_position.y, ((real32) dim.height) - player->size.height);
    player->position = new_position;
}

internal void
update_bad_guy(dodger_state *state, dodger_bad_guy *bad_guy, real32 dt) {
    v2 velocity = {};
    real32 speed = 100.f * dt;
    
    bad_guy->position.y += speed;
    
    v2i dim = state->world.dimensions;
    if (bad_guy->position.y > dim.height) {
        // Re-use bad guy
        init_bad_guy(state, bad_guy);
        state->score++;
        if (state->score > state->top_score) {
            state->top_score = state->score;
        }
    }
}

internal b32
check_for_collision(dodger_player *player, dodger_bad_guy *bad_guy) {
    b32 result = 0;
    
    box player_box = box{
        player->position.x,
        player->position.y,
        player->size.width,
        player->size.height,
    };
    box guy_box = box{
        bad_guy->position.x,
        bad_guy->position.y,
        bad_guy->size.width,
        bad_guy->size.height,
    };
    
    result = aabb_vs_aabb(player_box, guy_box);
    
    return result;
}

internal void
draw_player(dodger_player *player) {
    v4 color = make_color(0xffffffff);
    immediate_quad(player->position, add_v2(player->position, player->size), color, 1.f);
}


internal void
draw_bad_guy(dodger_bad_guy *bad_guy) {
    v4 color = make_color(0xff66ff66);
    immediate_quad(bad_guy->position, add_v2(bad_guy->position, bad_guy->size), color, 1.f);
}

internal void
draw_menu(dodger_state *state) {
    v2i dim = state->world.dimensions;
    v4 default_color = make_color(0xffffffff);
    v4 selected_color = make_color(0xffffff00);
    
    char *menu_title = state->game_mode == GameMode_GameOver ? "Game Over" : "Dodger";
    real32 menu_title_width = get_text_width(&state->assets.menu_title_font, menu_title);
    
    real32 y = dim.height * 0.2f;
    
    //
    // Title
    //
    //  Retry
    //  Quit
    //
    
    draw_text((dim.width - menu_title_width) / 2.f, y, (u8 *) menu_title, &state->assets.menu_title_font, default_color);
    
    char* menu_items[] = {"Retry", state->quit_was_selected ? "Are you sure?" : "Quit"};
    
    immediate_begin();
    
    y += dim.height * 0.25f;
    for (int menu_item = 0; menu_item < array_count(menu_items); ++menu_item) {
        
        char *text = menu_items[menu_item];
        real32 width = get_text_width(&state->assets.menu_item_font, text);
        v4 color = default_color;
        if (state->menu_selected_item == menu_item) {
            color = selected_color;
        }
        
        immediate_text((dim.width - width) / 2.f, y, (u8 *) text, &state->assets.menu_item_font, color, 1.f);
        
        y += (real32) state->assets.menu_item_font.line_height;
    }
    
    immediate_flush();
}

internal void
draw_game_view(dodger_state *state) {
    if (state->game_mode == GameMode_Playing) {
        immediate_begin();
        draw_player(&state->player);
        for (u32 bad_guy_index = 0; bad_guy_index < array_count(state->bad_guys); ++bad_guy_index) {
            draw_bad_guy(&state->bad_guys[bad_guy_index]);
        }
        immediate_flush();
        
        //
        // Draw HUD
        //
        
        v2i dim = state->world.dimensions;
        char buffer[256];
        // TODO: Platform specific sprintf()
        wsprintfA(buffer, "Top Score: %d\nScore: %d", (int) state->top_score, (int) state->score);
        
        draw_text(dim.width * 0.02f, dim.height * 0.05f, (u8 *) buffer, &state->assets.primary_font, make_color(0xffffffff));
    } else {
        draw_menu(state);
    }
}

internal void
dodger_menu_art(v2 min, v2 max) {
    
    v4 background = make_color(0xff141414);
    
    immediate_begin();
    
    immediate_quad(min, max, background, 1.f);
    
    {
        v4 color = make_color(0xff66ff66);
        v2 size = make_v2(24.f, 24.f);
        dodger_bad_guy guys[] = {
            dodger_bad_guy{make_v2(min.x + size.width * 5.f, max.y - max.y * .2f), size},
            dodger_bad_guy{make_v2(max.x - size.width * 3.f, max.y - max.y * .5f), size},
            dodger_bad_guy{make_v2(min.x + size.width * 1.2f, max.y - max.y * .6f), size},
            dodger_bad_guy{make_v2(max.x - size.width * 1.2f, max.y - max.y * .6f), size},
            dodger_bad_guy{make_v2(min.x + size.width * 1.4f, max.y - max.y * .33f), size},
            dodger_bad_guy{make_v2(max.x - size.width * 1.4f, max.y - max.y * .33f), size},
            dodger_bad_guy{make_v2(min.x + size.width * 1.2f, max.y - max.y * .5f), size},
            dodger_bad_guy{make_v2(max.x - size.width * 2.9f, max.y - max.y * .8f), size},
            dodger_bad_guy{make_v2(min.x + size.width * 1.9f, max.y - max.y * .8f), size},
            dodger_bad_guy{make_v2(max.x - size.width * 1.55f, max.y - max.y * .12f), size},
            dodger_bad_guy{make_v2(min.x + size.width * 8.55f, max.y - max.y * .12f), size},
            dodger_bad_guy{make_v2(max.x - size.width * 4.11f, max.y - max.y * .2f), size},
        };
        for (int i = 0; i < array_count(guys); ++i) {
            dodger_bad_guy guy = guys[i];
            immediate_quad(guy.position, add_v2(guy.position, guy.size), color, 1.f);
        }
    }
    
    {
        // Player
        v2 size = make_v2(24.f, 24.f);
        v2 position = make_v2((min.x + max.x - size.width) / 2.f, (min.y + max.y - size.height) / 2.f);
        v4 color = make_color(0xffffffff);
        immediate_quad(position, add_v2(position, size), color, 1.f);
    }
    
    immediate_flush();
}

internal void
dodger_game_update_and_render(game_memory *memory, game_input *input) {
    
    dodger_state *state = (dodger_state *) memory->permanent_storage;
    if (!memory->initialized) {
        assert(memory->permanent_storage_size == 0);
        assert(!memory->permanent_storage);
        
        // Init Dodger state
        state = (dodger_state *) game_alloc(memory, megabytes(64));
        
        dodger_assets assets = {};
        assets.primary_font = load_font("./data/fonts/Inconsolata-Regular.ttf", 24.f);
        assets.menu_title_font = load_font("./data/fonts/Inconsolata-Bold.ttf", 48.f);
        assets.menu_item_font = load_font("./data/fonts/Inconsolata-Regular.ttf", 32.f);
        
        dodger_world world = {};
        world.dimensions = memory->window_dimensions;
        
        state->game_mode = GameMode_Playing;
        state->assets = assets;
        state->world = world;
        state->score = 0;
        state->top_score = 0;
        
        init_player(state);
        init_bad_guys(state);
        
        // Set default mouse input values for this game
        input->mouse.sensitivity = .5f;
    }
    
    real32 dt = memory->dt;
    
    // NOTE(diego): Lock mouse to center of screen and use new position
    // to calculate velocity and move our player with it.
    {
        v2i new_mouse_position;
        platform_get_cursor_position(&new_mouse_position);
        
        input->mouse.velocity = sub_v2i(new_mouse_position, input->mouse.position);
        
        if (input->mouse.lock) {
            platform_set_cursor_position(memory->window_center);
            input->mouse.position = memory->window_center;
        } else {
            input->mouse.position = new_mouse_position;
        }
        platform_show_cursor(false);
    }
    
    state->world.dimensions = memory->window_dimensions;
    
    //
    // Update
    //
    if (state->game_mode == GameMode_Playing) {
        if (pressed(Button_Escape)) {
            state->game_mode = GameMode_Menu;
        } else {
            update_player(state, input, dt);
            for (u32 bad_guy_index = 0; bad_guy_index < array_count(state->bad_guys); ++bad_guy_index) {
                dodger_bad_guy *bad_guy = &state->bad_guys[bad_guy_index];
                update_bad_guy(state, bad_guy, dt);
                if (check_for_collision(&state->player, bad_guy)) {
                    state->game_mode = GameMode_GameOver;
                    break;
                }
            }
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
                    dodger_game_restart(state);
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
        
        if (state->quit_was_selected) {
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
