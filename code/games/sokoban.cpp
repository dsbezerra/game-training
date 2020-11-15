
internal void
draw_game_view(sokoban_state *state) {
    
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
    
}

internal void
sokoban_game_update_and_render(game_memory *memory, game_input *input) {
    
}
