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

enum tetromino {
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

enum tetris_move_direction {
    TetrisMoveDirection_Up, // Not used.
    TetrisMoveDirection_Down,
    TetrisMoveDirection_Right,
    TetrisMoveDirection_Left,
};

enum tetris_piece_rotation {
    TetrisPieceRotation_0,
    TetrisPieceRotation_1,
    TetrisPieceRotation_2,
    TetrisPieceRotation_3,
};

struct tetris_block {
    s8 x, y;
    b32 placed;
    v4 color;
};

struct tetris_piece {
    tetromino kind;
    tetris_piece_rotation rotation;
    tetris_block blocks[TETRIS_PIECE_BLOCK_COUNT];
    v4 color;
};

struct tetris_state {
    v2i dimensions;
    
    tetris_block grid[TETRIS_GRID_Y_COUNT][TETRIS_GRID_X_COUNT];
    
    tetris_piece current_piece;
    tetris_piece next_piece;
    
    real32 move_t;
    real32 move_t_target;
    real32 move_dt;
    
    game_mode game_mode;
    s8 menu_selected_item;
    b32 quit_was_selected;
};

internal void init_game(tetris_state *state);

internal void draw_game_view(tetris_state *state);
internal void tetris_menu_art(v2 min, v2 max);
internal void tetris_game_restart(tetris_state *state);
internal void tetris_game_update_and_render(game_memory *memory, game_input *input);
