/* date = April 20th 2021 8:10 pm */
/*
 
Training 12: Bejeweled

Description: The board is filled with seven different types of jewels. The player can swap two adjacent jewels to form a three-in-a-row, causing the jewels to disappear and the jewels on top of them to fall down. Creating chain reactions gives bonus points.

Variant: Different power ups for matching a particular jewel. Be able to sometimes swap jewels that are not adjacent to each other. Timed games.

*/

#define BEJEWELED_TITLE "Bejeweled"

#define BEJEWELED_GRID_COUNT 8

enum Bejeweled_Gem {
    BejeweledGem_None,
    BejeweledGem_Purple,
    BejeweledGem_Green,
    BejeweledGem_Orange,
    BejeweledGem_Yellow,
    BejeweledGem_Pink,
    BejeweledGem_Black,
    BejeweledGem_White,
    BejeweledGem_Count,
};

enum Bejeweled_Slot_Type {
    BejeweledSlotType_Normal,
};

struct Bejeweled_Gem_Sprite {
    Bejeweled_Gem gem;
    Sprite *sprite;
};

struct Bejeweled_Slot {
    Bejeweled_Slot_Type type;
    Bejeweled_Gem gem;
    
    u32 x;
    u32 y;
};

struct Bejeweled_State;

struct Bejeweled_Board {
    Bejeweled_State *state;
    Bejeweled_Slot slots[BEJEWELED_GRID_COUNT][BEJEWELED_GRID_COUNT];
};

struct Bejeweled_State {
    Game_Mode game_mode;
    Game_Memory *memory;
    
    Bejeweled_Gem_Sprite gems[BejeweledGem_Count-1];
    
    Bejeweled_Board board;
    
    // Spritesheet
    Spritesheet *main_sheet;
};

internal void clear_and_generate_board(Bejeweled_Board *board);
internal void clear_board(Bejeweled_Board *board);
internal void generate_board(Bejeweled_Board *board);

internal Bejeweled_Gem get_random_gem(Bejeweled_State *state);
internal Sprite * get_sprite(Bejeweled_State *state, Bejeweled_Gem gem);

internal void init_game(Bejeweled_State *state);
internal void update_game(Bejeweled_State *state, Game_Input *input);

internal void draw_game_view(Bejeweled_State *state);

internal void bejeweled_menu_art(App_State *state, Vector2 min, Vector2 max);
internal void bejeweled_game_restart(Bejeweled_State *state);
internal void bejeweled_game_update_and_render(Game_Memory *memory, Game_Input *input);
internal void bejeweled_game_free(Game_Memory *memory);
