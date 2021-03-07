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
    
    OthelloTileKind_Hover,
    OthelloTileKind_FindDirection,
    
    OthelloTileKind_Black,
    OthelloTileKind_White,
    
    OthelloTileKind_Count,
};

enum Othello_Side {
    OthelloSide_None,
    
    OthelloSide_Right,
    OthelloSide_Left,
    OthelloSide_Up,
    OthelloSide_Down,
    
    OthelloSide_UpRight,
    OthelloSide_UpLeft,
    
    OthelloSide_DownRight,
    OthelloSide_DownLeft,
    
    OthelloSide_Count,
};

enum Othello_Play_State {
    OthelloPlayState_None,
    
    OthelloPlayState_BlackTurn,
    OthelloPlayState_WhiteTurn,
    
    OthelloPlayState_BlackWin,
    OthelloPlayState_WhiteWin,
    
    OthelloPlayState_Count,
};

struct Othello_Assets {
    Loaded_Font board_font;
    Loaded_Font turn_font;
};

struct Othello_State;

struct Othello_Tile {
    Othello_Tile_Kind kind;
    s32 x;
    s32 y;
    
    char letter;
    char number;
    
    char coordinate[3];
};

struct Othello_Board {
    Othello_State *state;
    Othello_Tile tiles[OTHELLO_BOARD_COUNT][OTHELLO_BOARD_COUNT];
};

struct Othello_Coordinate {
    u32 x;
    u32 y;
};

struct Othello_Move {
    s32 x;
    s32 y;
    
    Othello_Tile origin;
    
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
    
    Othello_Assets assets;
    Othello_Board board;
    Othello_Move_List move_list;
    
    Othello_Tile_Kind current_player;
    
    Othello_Tile hovering_tile;
    Othello_Play_State play_state;
    
    Vector2i dimensions;
    
    s8 menu_selected_item;
    b32 quit_was_selected;
};

internal void init_game(Othello_State *state);
internal void update_game(Othello_State *state, Game_Input *input);
internal void update_hovering_tile(Othello_State *state);

internal void draw_board(Othello_State *state);
internal void draw_game_view(Othello_State *state);
internal void draw_hud(Othello_State *state);

internal void reset_board(Othello_Board *board);
internal void clear_board(Othello_Board *board);

internal Othello_Tile_Kind check_for_win(Othello_State *state);

internal void eat_tiles_for_move(Othello_Board *board, Othello_Move *move);
internal void eat_tiles_to_side_of_move(Othello_Board *board, Othello_Move *move, Othello_Side side);

internal Othello_Tile * find_tile_to_side_of_move(Othello_Board *board, Othello_Move *move, Othello_Side side);

internal void move_list_clear(Othello_State *state);
internal Othello_Move * move_list_find(Othello_State *state, Othello_Move move);
internal void move_list_add(Othello_State *state, Othello_Move move);
internal void move_list_add_if_unique(Othello_State *state, Othello_Move move);
internal void move_list_update(Othello_State *state);
internal void move_list_check(Othello_State *state, Othello_Tile *tile, s32 x, s32 y);
internal Othello_Move * move_list_valid(Othello_State *state, Othello_Tile *tile);

internal void make_move(Othello_State *state);

internal b32 is_empty(Othello_Board *board, u32 tile_x, u32 tile_y);
internal char get_move_letter(u32 tile_x);
internal char get_move_number(u32 tile_y);
internal Othello_Coordinate get_coordinate(char *coordinate);
internal Vector2 get_start_xy(Othello_State *state);
internal real32 get_tile_size(Othello_State *state);

internal void switch_turns(Othello_State *state);

internal void set_play_state(Othello_State *state, Othello_Play_State new_state);
internal void set_tile(Othello_Board *board, Othello_Tile_Kind kind, u32 tile_x, u32 tile_y);
internal void set_tile(Othello_Board *board, Othello_Tile_Kind kind, char *coordinate);
internal Othello_Tile * find_valid_move_for_tile(Othello_Board *board, Othello_Tile *tile, Othello_Tile tile_direction);

internal void othello_menu_art(App_State *state, Vector2 min, Vector2 max);
internal void othello_game_restart(Othello_State *state);
internal void othello_game_update_and_render(Game_Memory *memory, Game_Input *input);
internal void othello_game_free(Game_Memory *memory);
