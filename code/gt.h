
#include "gt_types.h"
#include "gt_shared.h"
#include "gt_math.h"
#include "gt_opengl.h"
#include "gt_shader.h"
#include "gt_font.h"
#include "gt_platform.h"
#include "gt_profiler.h"
#include "gt_collision.h"
#include "gt_draw.h"
#include "gt_mesh.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_FAILURE_STRINGS
#define STBI_ASSERT assert
#include "stb/stb_image.h"

struct Game_Time_Info {
    real32 dt;
    real32 current_time;
};

struct Game_Sound_Output_Buffer;

struct Game_Menu_Art {
    GLuint dodger;
    GLuint memory_puzzle;
    GLuint slide_puzzle;
    GLuint simon;
    GLuint nibbles;
    GLuint tetris;
    GLuint katamari;
};

struct Game_Button_State {
    b32 is_down;
    b32 changed;
};

enum Game_Menu_Item {
    GameMenuItem_Retry,
    GameMenuItem_Quit,
    
    GameMenuItem_Count,
};

enum Game_Mode {
    GameMode_Menu,
    GameMode_Playing,
    GameMode_GameOver,
};

enum Button {
    Button_Escape,
    Button_Enter,
    Button_Space,
    
    Button_Up,
    Button_Down,
    Button_Left,
    Button_Right,
    
    Button_W,
    Button_A,
    Button_S,
    Button_D,
    
    Button_Mouse1,
    
    Button_Count
};

struct Game_Mouse {
    Vector2i    velocity;
    Vector2i    position;
    real32      sensitivity;
    b32         lock;
};

struct Game_Input {
    Game_Mouse mouse;
    union {
        Game_Button_State buttons[Button_Count];
        struct {
            Game_Button_State escape;
            Game_Button_State enter;
            Game_Button_State space;
            
            Game_Button_State up;
            Game_Button_State down;
            Game_Button_State left;
            Game_Button_State right;
            
            Game_Button_State w;
            Game_Button_State a;
            Game_Button_State s;
            Game_Button_State d;
            
            Game_Button_State mouse1;
        };
    };
};

enum App_Mode {
    Mode_SelectingGame,
    Mode_PlayingGame,
    
    Mode_Count,
};

struct Game_Memory {
    Vector2i window_center;
    Vector2i window_dimensions;
    
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

struct App_State {
    Vector2i window_dimensions;
    Vector2i window_center;
    
    App_Mode current_mode;
    game current_game;
    game current_selecting_game;
    
    Game_Menu_Art menu_art;
    
    b32 initialized;
    
    Loaded_Font game_title_font;
    Game_Memory *memory;
};

#include "gt_camera.h" // TODO(diego): Refactor so this can be up there with other h files.
#include "gt_games.h"

internal void game_output_sound(Game_Sound_Output_Buffer *sound_buffer);
internal void game_update_and_render(App_State *state, Game_Memory *memory, Game_Input *input);

internal void *game_alloc(Game_Memory *memory, u64 size);
internal void game_free(Game_Memory *memory, game current_game);
internal void game_quit(App_State *state);

internal void advance_menu_choice(s8 *current_choice, s8 delta);

internal GLint load_menu_art(char *filename);

#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)
#define is_down(b) (input->buttons[b].is_down)

global_variable b32 global_vsync = false;
global_variable b32 global_lock_fps = false;

