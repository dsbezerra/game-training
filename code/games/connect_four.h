/* date = March 12th 2021 6:06 pm */
/*
 
Training 11: Connect Four

Description: Two players of different colors drop their tokens on an upright board. The player to make four tokens in a row, column, or diagonal wins. Creating an AI for this requires a simple minimax algorithm.

Variant: Different board sizes. Walls inside the board that appear when the spaces beneath them are filled.

*/

#define CONNECT_FOUR_TITLE "Connect Four"

struct Connect_Four_State {
    Game_Mode game_mode;
    
    Vector2i dimensions;
    
    s8 menu_selected_item;
    b32 quit_was_selected;
};

internal void init_game(Connect_Four_State *state);
internal void update_game(Connect_Four_State *state, Game_Input *input);

internal void draw_game_view(Connect_Four_State *state);
internal void draw_hud(Connect_Four_State *state);

internal void connect_four_menu_art(App_State *state, Vector2 min, Vector2 max);
internal void connect_four_game_restart(Connect_Four_State *state);
internal void connect_four_game_update_and_render(Game_Memory *memory, Game_Input *input);
internal void connect_four_game_free(Game_Memory *memory);
