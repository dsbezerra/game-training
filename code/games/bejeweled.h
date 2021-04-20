/* date = April 20th 2021 8:10 pm */
/*
 
Training 12: Bejeweled

Description: The board is filled with seven different types of jewels. The player can swap two adjacent jewels to form a three-in-a-row, causing the jewels to disappear and the jewels on top of them to fall down. Creating chain reactions gives bonus points.

Variant: Different power ups for matching a particular jewel. Be able to sometimes swap jewels that are not adjacent to each other. Timed games.

*/

#define BEJEWELED_TITLE "Bejeweled"

struct Bejeweled_State {
    Game_Mode game_mode;
    Game_Memory *memory;
    
    // TODO(diego): Fill
};

internal void init_game(Bejeweled_State *state);
internal void update_game(Bejeweled_State *state, Game_Input *input);

internal void draw_game_view(Bejeweled_State *state);

internal void bejeweled_menu_art(App_State *state, Vector2 min, Vector2 max);
internal void bejeweled_game_restart(Bejeweled_State *state);
internal void bejeweled_game_update_and_render(Game_Memory *memory, Game_Input *input);
internal void bejeweled_game_free(Game_Memory *memory);
