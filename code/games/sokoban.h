/* date = November 14th 2020 11:40 pm */
/*

Training 8: Sokoban

Description: The player is in a level with objects that need to be pushed over goals. The objects can only be pushed, they can't be pulled. This game does require some effort to design levels for, but Sokoban levels have been designed by others and published on the web.

Variant: Add all sorts of level gimmicks: teleport tiles, conveyor belts, buttons that open doors/bridges, buttons that need an object left on them to keep a door open.

*/

#define SOKOBAN_TITLE "Sokoban"

struct sokoban_tile {
    v3 position;
    v4 color;
};

struct sokoban_assets {
    GLint none;
};

struct sokoban_state {
    v2i dimensions;
    
    camera cam;
    
    sokoban_assets assets;
    
    sokoban_tile tiles[16][16];
    
    game_mode game_mode;
    s8 menu_selected_item;
    b32 quit_was_selected;
};

internal void init_game(sokoban_state *state);
internal void update_game(sokoban_state *state, game_input *input);

internal void draw_game_view(sokoban_state *state);

internal void sokoban_menu_art(v2 min, v2 max);
internal void sokoban_game_restart(sokoban_state *state);
internal void sokoban_game_update_and_render(game_memory *memory, game_input *input);
