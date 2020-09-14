/*

Training 2: Memory Puzzle

Description: A board full of overturned cards. There is a pair for each card. The player flips over two cards. If they match, then they stay overturned. Otherwise they flip back. The player needs to overturn all the cards in the fewest moves to win.

Variations: Provide "hints" in the form of four possible matching cards after the player flips the first one. Or, quickly overturn groups of cards at the beginning of the game.

*/

struct memory_puzzle_assets {
    loaded_font primary_font;
    loaded_font menu_title_font;
    loaded_font menu_item_font;
};

enum memory_card_kind {
    MemoryCard_None,
    MemoryCard_Donut,
    MemoryCard_Square,
    MemoryCard_Oval,
    MemoryCard_Lines,
    MemoryCard_Circle,
    MemoryCard_Diamond,
    MemoryCard_DoubleSquare,
    
    MemoryCard_Count,
};

struct memory_card {
    memory_card_kind kind;
    b32 flipped;
    v4 color;
};

struct memory_puzzle_world {
    memory_card field[10][6];
    v2i dimensions;
};

struct memory_puzzle_level {
    u32 max_tries;
    u32 tries;
};

struct memory_puzzle_state {
    game_mode game_mode;
    
    memory_puzzle_assets assets;
    memory_puzzle_world world;
    memory_puzzle_level current_level;
    
    s8 current_selected;
    memory_card *first_flipped;
    memory_card *second_flipped;
    
    real32 checking_cards_t;
    real32 checking_cards_target;
    
    u64 flip_count;
    
    s8 menu_selected_item;
    b32 quit_was_selected;
};


//internal void dodger_game_restart(dodger_state *state);

//internal void dodger_menu_art(v2i min, v2i max);
internal void memory_puzzle_game_update_and_render(game_memory *memory, game_input *input);

internal memory_puzzle_world init_world();

internal void draw_game_view(memory_puzzle_state *state);
internal void draw_game_over(memory_puzzle_state *state);
