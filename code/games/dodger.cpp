
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
update_player(dodger_state *state, game_input *input) {
    
    dodger_player *player = &state->player;
    
    v2i dim = state->world.dimensions;
    
    real32 mouse_sensitivity = .5f;
    v2 velocity = make_v2(mouse_sensitivity * input->mouse_velocity.x, mouse_sensitivity * input->mouse_velocity.y);
    
    real32 speed = 2.f;
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
update_bad_guy(dodger_state *state, dodger_bad_guy *bad_guy) {
    v2 velocity = {};
    real32 speed = 5.f;
    
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
dodger_game_update_and_render(game_memory *memory, game_input *input) {
    
    dodger_state *state = (dodger_state *) memory->permanent_storage;
    if (!memory->initialized) {
        assert(memory->permanent_storage_size == 0);
        assert(!memory->permanent_storage);
        
        state = (dodger_state *) game_alloc(memory, megabytes(64));
        
        dodger_assets assets = {};
        assets.primary_font = load_font("./data/fonts/Inconsolata-Regular.ttf", 24.f);
        
        dodger_world world = {};
        
        state->assets = assets;
        state->world = world;
        state->score = 0;
        state->top_score = 0;
        
        init_player(state);
        init_bad_guys(state);
    }
    
    v2i dim = memory->window_dimensions;
    state->world.dimensions = dim;
    
    //
    // Update
    //
    
    update_player(state, input);
    for (u32 bad_guy_index = 0; bad_guy_index < array_count(state->bad_guys); ++bad_guy_index) {
        update_bad_guy(state, &state->bad_guys[bad_guy_index]);
    }
    
    //
    // Draw World
    //
    
    immediate_begin();
    draw_player(&state->player);
    for (u32 bad_guy_index = 0; bad_guy_index < array_count(state->bad_guys); ++bad_guy_index) {
        draw_bad_guy(&state->bad_guys[bad_guy_index]);
    }
    immediate_flush();
    
    //
    // Draw HUD
    //
    char buffer[256];
    sprintf(buffer, "Top Score: %d\nScore: %d", (int) state->top_score, (int) state->score);
    
    draw_text(dim.width * 0.02f, dim.height * 0.05f, (u8 *) buffer, &state->assets.primary_font, make_color(0xffffffff));
}