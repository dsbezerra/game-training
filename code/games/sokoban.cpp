internal void
init_game(sokoban_state *state) {
    // TODO
}

internal void
draw_game_view(sokoban_state *state) {
    // TODO
}

internal void
sokoban_menu_art(app_state *state, v2 min, v2 max) {
    v4 background = make_color(0xFFFF00FF);
    immediate_begin();
    immediate_quad(min, max, background, 1.f);
    immediate_flush();
}

internal void
sokoban_game_restart(sokoban_state *state) {
    init_game(state);
}

internal void
sokoban_game_update_and_render(game_memory *memory, game_input *input) {
    sokoban_state *state = (sokoban_state *) memory->permanent_storage;
    if (!memory->initialized) {
        assert(memory->permanent_storage_size == 0);
        assert(!memory->permanent_storage);
        memory->initialized = true;
        
        state = (sokoban_state *) game_alloc(memory, megabytes(12));
        
        init_game(state);
    }
    state->dimensions = memory->window_dimensions;
    
    // TODO
}
