/* date = September 18th 2020 7:22 pm */
/*

Training 3: Slide Puzzle

Description: A board full of overturned cards. There is a pair for each card. The player flips over two cards. If they match, then they stay overturned. Otherwise they flip back. The player needs to overturn all the cards in the fewest moves to win.

Variations: Provide "hints" in the form of four possible matching cards after the player flips the first one. Or, quickly overturn groups of cards at the beginning of the game.

*/

#define SLIDE_PUZZLE_TITLE "Slide Puzzle"

struct slide_puzzle_assets {
    loaded_font primary_font;
};

struct slide_puzzle_state {
    slide_puzzle_assets assets;
    
    game_mode game_mode;
    s8 menu_selected_item;
    b32 quit_was_selected;
};

internal void draw_game_view(slide_puzzle_state *state);
internal void slide_puzzle_game_update_and_render(game_memory *memory, game_input *input);
