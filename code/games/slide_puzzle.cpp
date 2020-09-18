internal void
draw_game_view(slide_puzzle_state *state) {
    if (state->game_mode == GameMode_Playing) {
        
    } else {
        //draw_menu(SLIDE_PUZZLE_TITLE, state->world.dimensions, state->game_mode, state->menu_selected_item, state->quit_was_selected);
    }
}

internal void
slide_puzzle_game_update_and_render(game_memory *memory, game_input *input) {
    
    slide_puzzle_state *state = (slide_puzzle_state *) memory->permanent_storage;
    if (!memory->initialized) {
        assert(memory->permanent_storage_size == 0);
        assert(!memory->permanent_storage);
        
        state = (slide_puzzle_state *) game_alloc(memory, megabytes(12));
        
        slide_puzzle_assets assets = {};
        assets.primary_font = load_font("./data/fonts/Inconsolata-Regular.ttf", 24.f);
        
        state->game_mode = GameMode_Playing;
        state->assets = assets;
        
        memory->initialized = true;
    }
    
    draw_game_view(state);
}