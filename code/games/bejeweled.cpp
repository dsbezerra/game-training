internal void
init_game(Bejeweled_State *state) {
    state->game_mode = GameMode_Playing;
    state->memory->game_mode = GameMode_Playing;
    
    platform_show_cursor(true);
}

internal void
update_game(Bejeweled_State *state, Game_Input *input) {
    // TODO(diego): Implement
}

internal void
draw_game_view(Bejeweled_State *state) {
    // TODO(diego): Implement
}

internal void
bejeweled_menu_art(App_State *state, Vector2 min, Vector2 max) {
    // TODO(diego): Implement
}

internal void
bejeweled_game_restart(Bejeweled_State *state) {
    init_game(state);
}

internal void
bejeweled_game_update_and_render(Game_Memory *memory, Game_Input *input) {
    Bejeweled_State *state = (Bejeweled_State *) memory->permanent_storage;
    if (!memory->initialized) {
        assert(memory->permanent_storage_size == 0);
        assert(!memory->permanent_storage);
        memory->initialized = true;
        
        Memory_Index total_memory_size = kilobytes(16);
        Memory_Index total_available_size = total_memory_size - sizeof(Bejeweled_State);
        
        state = (Bejeweled_State *) game_alloc(memory, total_memory_size);
        state->memory = memory;
        
        init_game(state);
    }
    
    Simulate_Game sim = game_simulate(memory, input, state->game_mode);
    switch (sim.operation) {
        case SimulateGameOp_Update: {
            update_game(state, input);
        } break;
        
        case SimulateGameOp_Restart: {
            bejeweled_game_restart(state);
        } break;
        
        default: break;
    }
    
    //
    // Draw
    //
    draw_game_view(state);
}

internal void
bejeweled_game_free(Game_Memory *memory) {
    if (!memory) return;
    
    Bejeweled_State *state = (Bejeweled_State *) memory->permanent_storage;
    if (!state) return;
    
    // TODO(diego): If necessary.
}
