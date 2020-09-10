
#include "gt_types.h" 
#include "gt_shared.h"
#include "gt_math.h"
#include "gt_platform.h"
#include "gt_collision.h"
#include "gt_opengl.h"
#include "gt_shader.h"
#include "gt_font.h"
#include "gt_draw.h"

struct game_sound_output_buffer;

struct game_button_state {
    b32 is_down;
    b32 changed;
};

enum game_mode {
    GameMode_Menu,
    GameMode_Playing,
    GameMode_GameOver,
};

enum button {
    Button_Escape,
    Button_Enter,
    
    Button_Up,
    Button_Down,
    Button_Left,
    Button_Right,
    
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
            
            game_button_state up;
            game_button_state down;
            game_button_state left;
            game_button_state right;
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

#include "gt_games.h"

struct app_state {
    v2i window_dimensions;
    v2i window_center;
    
    mode current_mode;
    game current_game;
    game current_selecting_game;
    
    b32 initialized;
    
    loaded_font game_title_font;
    game_memory *memory;
};

internal void game_output_sound(game_sound_output_buffer *sound_buffer);
internal void game_update_and_render(app_state *state, game_memory *memory, game_input *input);

internal void *game_alloc(game_memory *memory, u64 size);
internal void game_free(game_memory *memory);
internal void game_quit(app_state *state);

#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)
#define is_down(b) (input->buttons[b].is_down)

