/* date = September 21st 2020 11:08 pm */
/*

Training 4: Nibbles

Description: A worm or snake constantly moves around the board. The player controls the direction the "head" of the worm moves, and the worm must try to eat apples that randomly appear. Eating an apply causes the worm to grow in length. The game ends if the worm crashes into the edge of the board or into itself.

Variants: Add walls to the level, instead of just a blank rectangle. Add power ups that the worm can pick up. Add bad guys that move around the board that the worm must avoid. Have two worms that the player must control simultaenously. Tron (see below) is a two-player variant of this game.

*/

#define NIBBLES_TITLE "Nibbles"

struct nibbles_state {
    v2i dimensions;
    
    game_mode game_mode;
    s8 menu_selected_item;
    b32 quit_was_selected;
};

internal void draw_game_view(nibbles_state *state);

internal void nibbles_menu_art(v2 min, v2 max);
internal void nibbles_game_restart(nibbles_state *state);
internal void nibbles_game_update_and_render(game_memory *memory, game_input *input);
