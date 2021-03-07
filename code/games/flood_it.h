/* date = March 7th 2021 2:20 pm */
/*

Training 10: Flood It

Description: A grid of six colors of tiles starts off randomly. The player can do a "flood fill" on the top left tile, changing the color of any adjacent tiles of thesame color. The player wins if they are able to make the entire board a single color within a certain number of moves.

Variants: Power ups gained when a certain tile is changed.

*/

#define FLOOD_IT_TITLE "Flood It"

struct Flood_It_Assets {
};

struct Flood_It_State;

struct Flood_It_State {
    Game_Mode game_mode;
    
    Flood_It_Assets assets;
    
    Vector2i dimensions;
    
    s8 menu_selected_item;
    b32 quit_was_selected;
};

internal void init_game(Flood_It_State *state);
internal void update_game(Flood_It_State *state, Game_Input *input);
internal void update_hovering_tile(Flood_It_State *state);

internal void draw_board(Flood_It_State *state);
internal void draw_game_view(Flood_It_State *state);
internal void draw_hud(Flood_It_State *state);

internal void flood_it_menu_art(App_State *state, Vector2 min, Vector2 max);
internal void flood_it_game_restart(Flood_It_State *state);
internal void flood_it_game_update_and_render(Game_Memory *memory, Game_Input *input);
internal void flood_it_game_free(Game_Memory *memory);
