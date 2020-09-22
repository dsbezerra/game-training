/* date = September 21st 2020 11:08 pm */
/*

Training 4: Nibbles

Description: A worm or snake constantly moves around the board. The player controls the direction the "head" of the worm moves, and the worm must try to eat apples that randomly appear. Eating an apply causes the worm to grow in length. The game ends if the worm crashes into the edge of the board or into itself.

Variants: Add walls to the level, instead of just a blank rectangle. Add power ups that the worm can pick up. Add bad guys that move around the board that the worm must avoid. Have two worms that the player must control simultaenously. Tron (see below) is a two-player variant of this game.

*/

#define NIBBLES_TITLE "Nibbles"

#define NIBBLES_WORLD_X_COUNT 32
#define NIBBLES_WORLD_Y_COUNT 16

#define NIBBLES_MOVING_AXIS_X 0
#define NIBBLES_MOVING_AXIS_Y 1

#define NIBBLES_MOVING_DIRECTION_NEGATIVE -1
#define NIBBLES_MOVING_DIRECTION_POSITIVE  1

enum nibbles_entity_kind {
    NibblesEntity_None,
    
    NibblesEntity_Apple,
    NibblesEntity_Snake,
    
    NibblesEntity_Count,
};

struct nibbles_entity {
    nibbles_entity_kind kind;
    u8 x;
    u8 y;
};

struct nibbles_state {
    v2i dimensions;
    
    u8 snake_length;
    nibbles_entity snake[NIBBLES_WORLD_Y_COUNT * NIBBLES_WORLD_X_COUNT];
    nibbles_entity apple;
    
    s8 moving_direction;
    u8 moving_axis;
    
    real32 last_eaten_apple_time;
    
    real32 step_t;
    real32 step_t_target;
    real32 step_dt;
    
    game_mode game_mode;
    s8 menu_selected_item;
    b32 quit_was_selected;
};

internal void draw_game_view(nibbles_state *state);

internal void nibbles_menu_art(v2 min, v2 max);
internal void nibbles_game_restart(nibbles_state *state);
internal void nibbles_game_update_and_render(game_memory *memory, game_input *input);
