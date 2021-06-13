/* date = September 23rd 2020 11:57 pm */
/*
 
Training 6: Tetris

Description: Shapes made up of four blocks fall from the top of the board. The player must rotate and place them to create full rows with no gaps. When a full row is made, the blocks in that row disappear and the blocks above it move down. The game ends if the board fills up.

Variant: Several Tetris variants are listed on Wikipedia. http://en.wikipedia.org/wiki/List_of_Tetris_variants

*/

#define TETRIS_TITLE "Tetris"

#define TETRIS_PIECE_BLOCK_COUNT 4
#define TETRIS_GRID_X_COUNT 10
#define TETRIS_GRID_Y_COUNT 20

enum Tetromino {
    Tetromino_None,
    Tetromino_I, 
    Tetromino_O,
    Tetromino_T, 
    Tetromino_S,
    Tetromino_Z,
    Tetromino_J,
    Tetromino_L,
    Tetromino_Count,
};

struct Tetromino_Stack {
    Tetromino data[Tetromino_Count-1];
    u32 size;
};

enum Tetris_Move_Direction {
    TetrisMoveDirection_Up, // Not used.
    TetrisMoveDirection_Down,
    TetrisMoveDirection_Right,
    TetrisMoveDirection_Left,
};

enum Tetris_Piece_Rotation {
    TetrisPieceRotation_0,
    TetrisPieceRotation_1,
    TetrisPieceRotation_2,
    TetrisPieceRotation_3,
};

struct Tetris_Block {
    s8 x, y;
    b32 placed;
    Vector4 color;
};

struct Tetris_Piece {
    Tetromino kind;
    Tetris_Piece_Rotation rotation;
    Tetris_Block blocks[TETRIS_PIECE_BLOCK_COUNT];
    Vector4 color;
};

struct Tetris_Assets {
    Loaded_Font primary_font;
    Loaded_Font hud_font;
};

struct Tetris_State {
    Game_Mode game_mode;
    Game_Memory *memory;
    
    Tetris_Assets assets;
    Tetris_Piece current_piece;
    Tetris_Piece next_piece;
    Tetris_Block grid[TETRIS_GRID_Y_COUNT][TETRIS_GRID_X_COUNT];
    
    Tetromino_Stack next_piece_stack;
    
    u64 score;
    u8 level; // Not used.
    
    real32 move_t;
    real32 move_t_target;
    real32 move_dt;
    
    Vector2i dimensions;
    s8 menu_selected_item;
    b32 quit_was_selected;
};

internal void init_game(Tetris_State *state);
internal b32 current_will_land(Tetris_State *state);

internal b32 is_piece_position_valid(Tetris_State *state, Tetris_Piece *piece);
internal b32 is_valid_position(Tetris_State *state, s8 x, s8 y, s8 adx, s8 ady);
internal b32 is_move_allowed(Tetris_State *state, s8 x, s8 y);
internal b32 is_inside_grid(s8 x, s8 y);
internal b32 is_row_complete(Tetris_State *state, s8 row);

internal void push_tetromino(Tetromino_Stack *stack, Tetromino value);
internal Tetromino pop_tetromino(Tetromino_Stack *stack);
internal void reset_tetromino_stack(Tetromino_Stack *stack);

internal Tetris_Piece rotate_I(Tetris_Piece *piece);
internal Tetris_Piece rotate_T(Tetris_Piece *piece);
internal Tetris_Piece rotate_S(Tetris_Piece *piece);
internal Tetris_Piece rotate_Z(Tetris_Piece *piece);
internal Tetris_Piece rotate_L(Tetris_Piece *piece);
internal Tetris_Piece rotate_J(Tetris_Piece *piece);

internal Tetris_Piece make_piece(Tetromino kind, s8 offset);
internal Tetris_Piece random_piece(Tetris_State *state);

internal void rotate_piece(Tetris_State *state);
internal void spawn_piece(Tetris_State *state, b32 make_current);
internal void move_piece(Tetris_State *state, Tetris_Move_Direction direction);
internal b32 place_piece(Tetris_State *state);

internal void draw_grid(Tetris_State *state);
internal void draw_current_piece(Tetris_State *state);
internal void draw_next_piece(Tetris_State *state);
internal void draw_hud(Tetris_State *state);
internal void draw_game_view(Tetris_State *state);

internal void tetris_menu_art(App_State *state, Vector2 min, Vector2 max);
internal void tetris_game_restart(Tetris_State *state);
internal void tetris_game_update_and_render(Game_Memory *memory, Game_Input *input);
