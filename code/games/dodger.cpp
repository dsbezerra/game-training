
internal void
dodger_game_restart(Dodger_State *state) {
    state->game_mode = GameMode_Playing;
    state->memory->game_mode = GameMode_Playing;
    state->score = 0;
    
    //
    // Re-init
    //
    
    init_player(state);
    init_bad_guys(state);
}

internal void
init_player(Dodger_State *state) {
    Dodger_Player player = {};
    
    Vector2i dim = state->world.dimensions;
    player.position = make_vector2(dim.width / 2.f, dim.height * .2f);
    player.size = make_vector2(25.f, 25.0f);
    
    state->player = player;
}

internal void
init_bad_guy(Dodger_State *state, Dodger_Bad_Guy *bad_guy) {
    Vector2i dim = state->world.dimensions;
    Vector2 position = make_vector2(random_real32_in_range(0.f, dim.width * 1.f), random_real32_in_range((real32) -dim.height, -dim.height / 2.f));
    real32 wh = random_real32_in_range(20.f, 50.f);
    Vector2 size = make_vector2(wh, wh);
    bad_guy->position = position;
    bad_guy->size = size;
}

internal void
init_bad_guys(Dodger_State *state) {
    for (u32 bad_guy_index = 0; 
         bad_guy_index < array_count(state->bad_guys);
         ++bad_guy_index) {
        Dodger_Bad_Guy *bad_guy = &state->bad_guys[bad_guy_index]; 
        init_bad_guy(state, bad_guy);
    }
}

internal void
update_player(Dodger_State *state, Game_Input *input) {
    
    Dodger_Player *player = &state->player;
    
    Vector2i dim = state->world.dimensions;
    
    Game_Mouse mouse = input->mouse;
    Vector2 velocity = make_vector2(mouse.sensitivity * mouse.velocity.x, mouse.sensitivity * mouse.velocity.y);
    
    real32 speed = 100.f * core.time_info.dt;
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
    
    Vector2 new_position = player->position + velocity;
    new_position.x = clampf(0.f, new_position.x, ((real32) dim.width) - player->size.width);
    new_position.y = clampf(0.f, new_position.y, ((real32) dim.height) - player->size.height);
    player->position = new_position;
}

internal void
update_bad_guy(Dodger_State *state, Dodger_Bad_Guy *bad_guy) {
    Vector2 velocity = {};
    real32 speed = 100.f * core.time_info.dt;
    
    bad_guy->position.y += speed;
    
    Vector2i dim = state->world.dimensions;
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
check_for_collision(Dodger_Player *player, Dodger_Bad_Guy *bad_guy) {
    b32 result = 0;
    
    Box player_box = Box{
        player->position.x,
        player->position.y,
        player->size.width,
        player->size.height,
    };
    Box guy_box = Box{
        bad_guy->position.x,
        bad_guy->position.y,
        bad_guy->size.width,
        bad_guy->size.height,
    };
    
    result = aabb_vs_aabb(player_box, guy_box);
    
    return result;
}

internal void
draw_player(Dodger_Player *player) {
    Vector4 color = make_color(0xffffffff);
    immediate_quad(player->position, player->position + player->size, color);
}

internal void
draw_bad_guy(Dodger_Bad_Guy *bad_guy) {
    Vector4 color = make_color(0xff66ff66);
    immediate_quad(bad_guy->position, bad_guy->position + bad_guy->size, color);
}

internal void
draw_game_view(Dodger_State *state) {
    
    game_frame_begin(state->world.dimensions.width, state->world.dimensions.height);
    
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
        
        Vector2i dim = state->world.dimensions;
        char buffer[256];
        // TODO: Platform specific sprintf()
        wsprintfA(buffer, "Top Score: %d\nScore: %d", (int) state->top_score, (int) state->score);
        
        draw_text(dim.width * 0.02f, dim.height * 0.05f, (u8 *) buffer, &state->assets.primary_font, make_color(0xffffffff));
    } else {
        draw_menu(DODGER_TITLE, state->memory);
    }
}

internal void
dodger_menu_art(App_State *state, Vector2 min, Vector2 max) {
    immediate_begin();
    immediate_textured_quad(min, max, state->menu_art.dodger);
    immediate_flush();
}

internal void
dodger_game_update_and_render(Game_Memory *memory, Game_Input *input) {
    
    Dodger_State *state = (Dodger_State *) memory->permanent_storage;
    if (!memory->initialized) {
        assert(memory->permanent_storage_size == 0);
        assert(!memory->permanent_storage);
        
        // Init Dodger state
        state = (Dodger_State *) game_alloc(memory, megabytes(64));
        state->memory = memory;
        
        Dodger_Assets assets = {};
        assets.primary_font = load_font("./data/fonts/Inconsolata-Regular.ttf", 24.f);
        
        Dodger_World world = {};
        world.dimensions = memory->window_dimensions;
        
        state->memory = memory;
        state->game_mode = GameMode_Playing;
        state->assets = assets;
        state->world = world;
        state->score = 0;
        state->top_score = 0;
        
        init_player(state);
        init_bad_guys(state);
        
        // Set default mouse input values for this game
        input->mouse.sensitivity = .5f;
        platform_show_cursor(false);
    }
    
    // NOTE(diego): Lock mouse to center of screen and use new position
    // to calculate velocity and move our player with it.
    {
        Vector2i new_mouse_position;
        platform_get_cursor_position(&new_mouse_position);
        
        input->mouse.velocity = new_mouse_position - input->mouse.position;
        
        if (input->mouse.lock) {
            platform_set_cursor_position(memory->window_center);
            input->mouse.position = memory->window_center;
        } else {
            input->mouse.position = new_mouse_position;
        }
    }
    
    state->world.dimensions = memory->window_dimensions;
    
    //
    // Update
    //
    Simulate_Game sim = game_simulate(memory, input, state->game_mode);
    switch (sim.operation) {
        case SimulateGameOp_Update: {
            update_player(state, input);
            for (u32 bad_guy_index = 0; bad_guy_index < array_count(state->bad_guys); ++bad_guy_index) {
                Dodger_Bad_Guy *bad_guy = &state->bad_guys[bad_guy_index];
                update_bad_guy(state, bad_guy);
                if (check_for_collision(&state->player, bad_guy)) {
                    state->game_mode = GameMode_GameOver;
                    break;
                }
            }
        } break;
        
        case SimulateGameOp_Restart: {
            dodger_game_restart(state);
        } break;
        
        default: break;
    }
    
    //
    // Draw
    //
    
    draw_game_view(state);
}
