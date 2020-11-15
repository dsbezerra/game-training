
#include "gt_types.h"
#include "gt_shared.h"
#include "gt_math.h"
#include "gt_opengl.h"
#include "gt_shader.h"
#include "gt_font.h"
#include "gt_platform.h"
#include "gt_profiler.h"
#include "gt_collision.h"
#include "gt_camera.h"
#include "gt_draw.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_FAILURE_STRINGS
#define STBI_ASSERT assert
#include "stb/stb_image.h"

struct game_time_info {
    real32 dt;
    real32 current_time;
};

struct game_sound_output_buffer;

struct game_menu_art {
    GLuint dodger;
    GLuint memory_puzzle;
    GLuint slide_puzzle;
    GLuint simon;
    GLuint nibbles;
    GLuint tetris;
    GLuint katamari;
};

struct game_button_state {
    b32 is_down;
    b32 changed;
};

enum game_menu_item {
    GameMenuItem_Retry,
    GameMenuItem_Quit,
    
    GameMenuItem_Count,
};

enum game_mode {
    GameMode_Menu,
    GameMode_Playing,
    GameMode_GameOver,
};

enum button {
    Button_Escape,
    Button_Enter,
    Button_Space,
    
    Button_Up,
    Button_Down,
    Button_Left,
    Button_Right,
    
    Button_Mouse1,
    
    Button_Count
};

struct game_mouse {
    v2i    velocity;
    v2i    position;
    real32 sensitivity;
    b32    lock;
};

struct game_input {
    game_mouse mouse;
    union {
        game_button_state buttons[Button_Count];
        struct {
            game_button_state escape;
            game_button_state enter;
            game_button_state space;
            
            game_button_state up;
            game_button_state down;
            game_button_state left;
            game_button_state right;
            
            game_button_state mouse1;
        };
    };
};

enum mode {
    Mode_SelectingGame,
    Mode_PlayingGame,
    
    Mode_Count,
};

struct game_memory {
    v2i window_center;
    v2i window_dimensions;
    
    b32 initialized;
    b32 asked_to_quit;
    
    u64 permanent_storage_size;
    void *permanent_storage;
};

enum game {
    Game_None,
    Game_Dodger,
    Game_MemoryPuzzle,
    Game_SlidePuzzle,
    Game_Simon,
    Game_Nibbles,
    Game_Tetris,
    Game_Katamari,
    Game_Sokoban,
    
    Game_Count,
};

struct app_state {
    v2i window_dimensions;
    v2i window_center;
    
    mode current_mode;
    game current_game;
    game current_selecting_game;
    
    game_menu_art menu_art;
    
    b32 initialized;
    
    loaded_font game_title_font;
    game_memory *memory;
};

#include "gt_games.h"

internal void game_output_sound(game_sound_output_buffer *sound_buffer);
internal void game_update_and_render(app_state *state, game_memory *memory, game_input *input);

internal void *game_alloc(game_memory *memory, u64 size);
internal void game_free(game_memory *memory, game current_game);
internal void game_quit(app_state *state);

internal void advance_menu_choice(s8 *current_choice, s8 delta);

internal GLint load_menu_art(char *filename);

#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)
#define is_down(b) (input->buttons[b].is_down)

global_variable b32 global_vsync = false;
global_variable b32 global_lock_fps = false;

