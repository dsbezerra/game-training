/* date = March 12th 2021 6:06 pm */
/*
 
Training 11: Connect Four

Description: Two players of different colors drop their tokens on an upright board. The player to make four tokens in a row, column, or diagonal wins. Creating an AI for this requires a simple minimax algorithm.

Variant: Different board sizes. Walls inside the board that appear when the spaces beneath them are filled.

*/

#define CONNECT_FOUR_TITLE "Connect Four"

#define CONNECT_FOUR_X_COUNT 7
#define CONNECT_FOUR_Y_COUNT 6

enum Connect_Four_Tile_Kind {
    ConnectFourTileKind_None,
    ConnectFourTileKind_Red,
    ConnectFourTileKind_Black,
    ConnectFourTileKind_Count,
};

enum Connect_Four_Play_State {
    ConnectFourPlayState_RedTurn,
    ConnectFourPlayState_BlackTurn,
    ConnectFourPlayState_Count,
};

enum Connect_Four_Winner {
    ConnectFourWinner_None,
    ConnectFourWinner_Red,
    ConnectFourWinner_Black,
    ConnectFourWinner_Tie,
};

struct Connect_Best_Move {
    real32 score;
    s32 x;
    s32 y;
};

struct Connect_Four_Move {
    s32 x;
    s32 y;
};

struct Connect_Four_Tile {
    Connect_Four_Tile_Kind kind;
    s32 board_x;
    s32 board_y;
    
    real32 radius;
    
    Vector2 min;
    Vector2 max;
    Vector2 center;
    
    Vector2 size;
    
    Vector4 color;
};

struct Connect_Four_State;

struct Connect_Four_Board {
    Connect_Four_State *state;
    Connect_Four_Tile tiles[CONNECT_FOUR_X_COUNT][CONNECT_FOUR_Y_COUNT];
};

struct Connect_Four_State {
    Game_Mode game_mode;
    Game_Memory *memory;
    
    Connect_Four_Play_State play_state;
    
    Connect_Four_Tile_Kind ai_player;
    Connect_Four_Tile_Kind player;
    Connect_Four_Tile_Kind current_player;
    
    Connect_Four_Board board;
    
    Connect_Four_Tile dragging_tile;
    
    Vector2i mouse_position;
};

internal void init_game(Connect_Four_State *state);
internal void update_game(Connect_Four_State *state, Game_Input *input);
internal void update_dragging_tile(Connect_Four_State *state, Game_Input *input);

internal void clear_board(Connect_Four_Board *board);
internal void set_tile(Connect_Four_Board *board, Connect_Four_Tile_Kind kind, u32 tile_x, u32 tile_y);
internal void set_tile(Connect_Four_Tile *tile, Connect_Four_Tile_Kind kind);
internal void switch_turns(Connect_Four_State *state);
internal void make_move(Connect_Four_Board *board, Connect_Four_Tile_Kind kind, s32 x);
internal void copy_board(Connect_Four_Board *dest, Connect_Four_Board *src);

internal Connect_Four_Move get_lowest_empty_move(Connect_Four_Board *board, s32 x);
internal b32 is_tile_empty(Connect_Four_Board *board, u32 tile_x, u32 tile_y);
internal b32 is_inside_player(Connect_Four_State *state);
internal b32 is_board_full(Connect_Four_Board *board);
internal b32 is_valid_move(Connect_Four_Board *board, s32 x);
internal real32 get_tile_size(Vector2i dimensions);
internal Vector4 get_tile_color(Connect_Four_Tile_Kind kind);
internal b32 has_n_connected(Connect_Four_Board *board, Connect_Four_Tile_Kind kind, u32 start_x, u32 start_y, u32 advance_x, u32 advance_y, u32 n);
internal b32 has_four_connected(Connect_Four_Board *board, Connect_Four_Tile_Kind kind, u32 start_x, u32 start_y, u32 advance_x, u32 advance_y);

internal Connect_Four_Winner check_win(Connect_Four_Board *board);
internal b32 check_win(Connect_Four_Board *board, Connect_Four_Tile_Kind kind);

internal void best_move(Connect_Four_Board *board, Connect_Four_Tile_Kind player);
internal void minimax(Connect_Four_Board *board, u32 depth, s32 *potential_moves);

internal void draw_game_view(Connect_Four_State *state);
internal void draw_board(Connect_Four_State *state);
internal void draw_hud(Connect_Four_State *state);

internal void connect_four_menu_art(App_State *state, Vector2 min, Vector2 max);
internal void connect_four_game_restart(Connect_Four_State *state);
internal void connect_four_game_update_and_render(Game_Memory *memory, Game_Input *input);
internal void connect_four_game_free(Game_Memory *memory);