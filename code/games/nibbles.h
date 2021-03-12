/* date = September 21st 2020 11:08 pm */
/*

Training 5: Nibbles

Description: A worm or snake constantly moves around the board. The player controls the direction the "head" of the worm moves, and the worm must try to eat apples that randomly appear. Eating an apply causes the worm to grow in length. The game ends if the worm crashes into the edge of the board or into itself.

Variants: Add walls to the level, instead of just a blank rectangle. Add power ups that the worm can pick up. Add bad guys that move around the board that the worm must avoid. Have two worms that the player must control simultaenously. Tron (see below) is a two-player variant of this game.

*/

#define NIBBLES_TITLE "Nibbles"

#define NIBBLES_WORLD_X_COUNT 32
#define NIBBLES_WORLD_Y_COUNT 16

enum Nibbles_Snake_Direction {
    NibblesSnakeDirection_None,
    NibblesSnakeDirection_Up,
    NibblesSnakeDirection_Down,
    NibblesSnakeDirection_Left,
    NibblesSnakeDirection_Right,
};

enum Nibbles_Entity_Kind {
    NibblesEntity_None,
    
    NibblesEntity_Apple,
    NibblesEntity_Snake,
    
    NibblesEntity_Count,
};

struct Nibbles_Entity {
    Nibbles_Entity_Kind kind;
    u8 x;
    u8 y;
};

struct Nibbles_State {
    Game_Mode game_mode;
    Game_Memory *memory;
    
    Vector2i dimensions;
    
    u8 snake_length;
    Nibbles_Entity snake[NIBBLES_WORLD_Y_COUNT * NIBBLES_WORLD_X_COUNT];
    Nibbles_Snake_Direction direction;
    
    Nibbles_Entity apple;
    
    real32 last_eaten_apple_time;
    
    real32 step_t;
    real32 step_t_target;
    real32 step_dt;
};

internal Nibbles_Entity make_entity(Nibbles_Entity_Kind kind, u8 x, u8 y);
internal void init_board(Nibbles_State *state);
internal void init_game(Nibbles_State *state);
internal b32 is_occupied(Nibbles_State *state, u8 x, u8 y);
internal void advance_snake(Nibbles_State *state);

internal void draw_apple(Nibbles_State *state, Nibbles_Entity *apple);
internal void draw_snake(Nibbles_State *state);
internal void draw_board(Nibbles_State *state);
internal void draw_game_view(Nibbles_State *state);

internal void nibbles_menu_art(App_State *state, Vector2 min, Vector2 max);
internal void nibbles_game_restart(Nibbles_State *state);
internal void nibbles_game_update_and_render(Game_Memory *memory, Game_Input *input);
