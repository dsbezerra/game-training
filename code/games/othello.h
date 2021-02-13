/* date = February 12th 2021 9:16 pm */
/*

Training 9: Othello

Description: On a grid, a black and white player places tiles of their color on the board. The opponent's tiles between the newly placed tile and that player's existing tiles are flipped to become the color of the player's tiles. The game ends when the board fills up and the player with the most tiles of their color wins.

Variant: Three player Othello with three different colors. Non-square boards.

*/

#define OTHELLO_TITLE "Othello"

#define OTHELLO_BOARD_COUNT 8

enum Othello_Tile_Kind {
    OthelloTileKind_None,
    
    OthelloTileKind_Black,
    OthelloTileKind_White,
    
    OthelloTileKind_Count,
};

struct Othello_Tile_Find_Direction {
    s32 x;
    s32 y;
};

struct Othello_State;

struct Othello_Tile {
    Othello_Tile_Kind kind;
    u32 x;
    u32 y;
};

struct Othello_Board {
    Othello_State *state;
    Othello_Tile tiles[OTHELLO_BOARD_COUNT][OTHELLO_BOARD_COUNT];
};

struct Othello_Move {
    u32 x;
    u32 y;
    
    Othello_Move *next;
};

struct Othello_Move_List {
    Othello_Move *first;
    Othello_Move *head;
    u32 size;
};

struct Othello_State {
    Game_Mode game_mode;
    
    Memory_Arena move_arena;
    
    Othello_Board board;
    Othello_Move_List move_list;
    
    Othello_Tile_Kind current_player;
    
    Vector2i dimensions;
    
    s8 menu_selected_item;
    b32 quit_was_selected;
};

internal void init_game(Othello_State *state);
internal void update_game(Othello_State *state, Game_Input *input);

internal void draw_board(Othello_State *state);
internal void draw_game_view(Othello_State *state);

internal void reset_board(Othello_Board *board);
internal void clear_board(Othello_Board *board);

internal void move_list_clear(Othello_State *state);
internal Othello_Move * move_list_find(Othello_State *state, Othello_Move move);
internal void move_list_add(Othello_State *state, Othello_Move move);
internal void move_list_add_if_unique(Othello_State *state, Othello_Move move);
internal void move_list_update(Othello_State *state);
internal void move_list_check(Othello_State *state, Othello_Tile *tile, s32 x, s32 y);

internal b32 is_empty(Othello_Board *board, u32 tile_x, u32 tile_y);

internal void set_tile(Othello_Board *board, Othello_Tile_Kind kind, u32 tile_x, u32 tile_y);
internal Othello_Tile * find_valid_move_for_tile(Othello_Board *board, Othello_Tile *tile, Othello_Tile_Find_Direction direction);

internal void othello_menu_art(App_State *state, Vector2 min, Vector2 max);
internal void othello_game_restart(Othello_State *state);
internal void othello_game_update_and_render(Game_Memory *memory, Game_Input *input);
internal void othello_game_free(Game_Memory *memory);
