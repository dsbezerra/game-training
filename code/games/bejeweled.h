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

struct Bejeweled_Sprite_UV {
    union {
        struct {
            Vector2 _00;
            Vector2 _10;
            Vector2 _01;
            Vector2 _11;
        };
        Vector2 uvs[4];
    };
};

struct Bejeweled_Slot {
    Bejeweled_Slot_Type type;
    Bejeweled_Gem gem;
    
    u32 x;
    u32 y;
    
    real32 tile_size;
    real32 half_tile_size;
    
    Vector2 center;
    
    Bejeweled_Sprite_UV normal;
    Bejeweled_Sprite_UV swapping; // NOTE(diego): Not used. 
};

struct Bejeweled_Tile {
    s32 x;
    s32 y;
};

struct Bejeweled_Gem_List {
    u32 num_gems;
    Bejeweled_Gem *gems;
};

enum Bejeweled_Swap_State {
    BejeweledSwapState_Idle,
    BejeweledSwapState_From,
    BejeweledSwapState_To,
    BejeweledSwapState_Prepared,
    BejeweledSwapState_Swapping,
};

struct Bejeweled_Gem_Swap {
    Bejeweled_Swap_State state;
    Bejeweled_Tile from;
    Bejeweled_Tile to;
    
    Vector2 from_center;
    Vector2 to_center;
    
    real32 t;
    real32 duration;
    
    b32 valid;
    b32 reversing;
};

struct Bejeweled_State;

struct Bejeweled_Board {
    Bejeweled_State *state;
    Bejeweled_Slot slots[BEJEWELED_GRID_COUNT][BEJEWELED_GRID_COUNT];
};

struct Bejeweled_State {
    Game_Mode game_mode;
    Game_Memory *memory;
    
    Memory_Arena board_arena;
    
    Bejeweled_Gem_Sprite gems[BejeweledGem_Count-1];
    
    Bejeweled_Board board;
    
    Loaded_Sound swap_sound;
    Loaded_Sound invalid_swap_sound;
    
    Bejeweled_Gem_Swap swap;
    
    // Spritesheet
    Spritesheet *main_sheet;
    
    Vector2i mouse_position;
    
    // Swipe stuff
    real32 pressed_t;
    Vector2i pressed_position;
};

internal void clear_and_generate_board(Bejeweled_Board *board);
internal void clear_board(Bejeweled_Board *board);
internal void generate_board(Bejeweled_Board *board);
internal Bejeweled_Gem_List possible_gems_for_slot(Bejeweled_Board *board, u32 x, u32 y);

internal void prepare_swap(Bejeweled_State *state, Bejeweled_Gem_Swap *swap);
internal void reverse_swap(Bejeweled_Gem_Swap *swap);

internal void do_swap(Bejeweled_State *state);
internal void clear_swap(Bejeweled_Gem_Swap *swap);
internal void swap_slots(Bejeweled_Slot *slot_a, Bejeweled_Slot *slot_b);
internal void copy_slot(Bejeweled_Slot *slot_dest, Bejeweled_Slot slot_source);
internal b32 is_swap_possible(Bejeweled_Gem_Swap swap);
internal b32 is_swap_valid(Bejeweled_Board *board, Bejeweled_Gem_Swap swap);
internal b32 is_tile_valid(Bejeweled_Tile tile);

internal b32 has_chain(Bejeweled_Board *board, u32 x, u32 y);

internal Bejeweled_Gem get_random_gem(Bejeweled_State *state);
internal Sprite * get_sprite(Bejeweled_State *state, Bejeweled_Gem gem);
internal Bejeweled_Gem get_gem_at(Bejeweled_Board *board, s32 x, s32 y);
internal Bejeweled_Slot * get_slot_at(Bejeweled_Board *board, s32 x, s32 y);
internal void random_gem_for_slot(Bejeweled_Board *board, Bejeweled_Slot *slot);
internal Bejeweled_Tile get_tile_under_xy(Bejeweled_State *state, s32 x, s32 y);
internal Vector2 get_start_xy(Vector2i dim, real32 tile_size);

internal void init_game(Bejeweled_State *state);
internal void update_game(Bejeweled_State *state, Game_Input *input);
internal void handle_mouse(Bejeweled_State *state, Game_Input *input);

internal void handle_swap(Bejeweled_State *state);

internal void draw_game_view(Bejeweled_State *state);

internal void bejeweled_menu_art(App_State *state, Vector2 min, Vector2 max);
internal void bejeweled_game_restart(Bejeweled_State *state);
internal void bejeweled_game_update_and_render(Game_Memory *memory, Game_Input *input);
internal void bejeweled_game_free(Game_Memory *memory);
