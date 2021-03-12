
#include "gt_types.h"
#include "gt_memory.h"
#include "gt_math.h"
#include "gt_platform.h"
#include "gt_shared.h"
#include "gt_catalog.h"
#include "gt_hotloader.h"
#include "gt_opengl.h"
#include "gt_shader.h"
#include "gt_font.h"
#include "gt_audio.h"
#include "gt_profiler.h"
#include "gt_collision.h"
#include "gt_mesh.h"
#include "gt_draw.h"
#include "gt_wav_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_FAILURE_STRINGS
#define STBI_ASSERT assert
#include "stb/stb_image.h"

#include "stb/stretchy_buffer.h"

struct Debug_Draw {
    b32 draw_shadow_map;
};

struct Game_Time_Info {
    u64 start_counter;
    real32 dt;
    real32 current_time;
};

struct Game_Core {
    Game_Time_Info time_info;
};

struct Game_Sound_Buffer {
    int size; // buffer size in bytes
    int num_channels;
    int samples_per_second;
    int bytes_per_sample;
    int running_sample_index;
    
    s16 *samples;
    int samples_to_write;
};

struct Game_Menu_Art {
    GLuint dodger;
    GLuint memory_puzzle;
    GLuint slide_puzzle;
    GLuint simon;
    GLuint nibbles;
    GLuint tetris;
    GLuint katamari;
    GLuint sokoban;
    GLuint othello;
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
    Button_Control,
    
    Button_Up,
    Button_Down,
    Button_Left,
    Button_Right,
    
    Button_W,
    Button_A,
    Button_S,
    Button_D,
    
    Button_Y,
    Button_Z,
    
    Button_Mouse1,
    
    Button_F1,
    Button_F2,
    
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
    b32 alt_is_down;
    union {
        Game_Button_State buttons[Button_Count];
        struct {
            Game_Button_State escape;
            Game_Button_State enter;
            Game_Button_State space;
            Game_Button_State control;
            
            Game_Button_State up;
            Game_Button_State down;
            Game_Button_State left;
            Game_Button_State right;
            
            Game_Button_State w;
            Game_Button_State a;
            Game_Button_State s;
            Game_Button_State d;
            
            Game_Button_State y;
            Game_Button_State z;
            
            Game_Button_State mouse1;
            
            Game_Button_State f1;
            Game_Button_State f2;
        };
    };
};

enum App_Mode {
    Mode_SelectingGame,
    Mode_PlayingGame,
    
    Mode_Count,
};

struct Game_Memory {
    Game_Mode game_mode;
    
    Vector2i window_center;
    Vector2i window_dimensions;
    
    b32 initialized;
    b32 asked_to_quit;
    b32 quit_was_selected;
    
    s8 menu_selected_item;
    
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
    Game_Othello,
    Game_FloodIt,
    Game_ConnectFour,
    
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

enum Simulate_Game_Op {
    SimulateGameOp_None,
    
    SimulateGameOp_Update,
    SimulateGameOp_Restart,
    SimulateGameOp_Quit,
    
    SimulateGameOp_Count,
};
struct Simulate_Game {
    Simulate_Game_Op operation;
};

#include "gt_camera.h" // TODO(diego): Refactor so this can be up there with other h files.
#include "gt_games.h"

internal void game_frame_begin(int width, int height);
internal void game_output_sound(Game_Sound_Buffer *sound_buffer);
internal void game_update_and_render(App_State *state, Game_Memory *memory, Game_Input *input);
internal Simulate_Game game_simulate(Game_Memory *memory, Game_Input *input, Game_Mode &game_mode);

internal void *game_alloc(Game_Memory *memory, u64 size);
internal void game_free(Game_Memory *memory, game current_game);
internal void game_quit(App_State *state);

internal void advance_menu_choice(s8 *current_choice, s8 delta);

internal void my_hotloader_callback(Asset_Change *change, b32 handled);

internal GLint load_menu_arts(char *filename);

internal real32 get_time();

#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)
#define is_down(b) (input->buttons[b].is_down)

global_variable b32 global_vsync = false;
global_variable b32 global_lock_fps = false;

global_variable Opengl *open_gl;
