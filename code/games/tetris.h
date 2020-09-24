/* date = September 23rd 2020 11:57 pm */
/*
 
Training 6: Tetris

Description: Shapes made up of four blocks fall from the top of the board. The player must rotate and place them to create full rows with no gaps. When a full row is made, the blocks in that row disappear and the blocks above it move down. The game ends if the board fills up.

Variant: Several Tetris variants are listed on Wikipedia. http://en.wikipedia.org/wiki/List_of_Tetris_variants

*/

#define TETRIS_TITLE "Tetris"

struct tetris_state {
    v2i dimensions;
    
    game_mode game_mode;
    s8 menu_selected_item;
    b32 quit_was_selected;
};

internal void tetris_menu_art(v2 min, v2 max);
internal void tetris_game_restart(tetris_state *state);
internal void tetris_game_update_and_render(game_memory *memory, game_input *input);
