/*

Training 2: Memory Puzzle

Description: A board full of overturned cards. There is a pair for each card. The player flips over two cards. If they match, then they stay overturned. Otherwise they flip back. The player needs to overturn all the cards in the fewest moves to win.

Variations: Provide "hints" in the form of four possible matching cards after the player flips the first one. Or, quickly overturn groups of cards at the beginning of the game.

*/

#define MEMORY_PUZZLE_TITLE "Memory Puzzle"

struct Memory_Puzzle_Assets {
    Loaded_Font primary_font;
};

enum Memory_Card_Kind {
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

struct Memory_Card {
    Memory_Card_Kind kind;
    b32 flipped;
    Vector4 color;
};

struct Memory_Puzzle_World {
    Memory_Card field[10][6];
    Vector2i dimensions;
};

struct Memory_Puzzle_Level {
    u32 max_tries;
    u32 tries;
};

struct Memory_Puzzle_State {
    Game_Mode game_mode;
    Game_Memory *memory;
    
    Memory_Puzzle_Assets assets;
    Memory_Puzzle_World world;
    Memory_Puzzle_Level current_level;
    
    s8 current_selected;
    Memory_Card *first_flipped;
    Memory_Card *second_flipped;
    
    real32 checking_cards_t;
    real32 checking_cards_target;
    
    u64 flip_count;
};

internal Memory_Puzzle_World init_world();

internal void draw_game_view(Memory_Puzzle_State *state);
internal void memory_puzzle_game_update_and_render(Game_Memory *memory, Game_Input *input);
// TODO(diego): Add prototypes.
