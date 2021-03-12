/* date = September 19th 2020 9:12 pm */
/*
 
Training 4: Simon

Description: Four colored buttons light up in a specific pattern. After displaying the pattern, the player must repeat the pattern by clicking the buttons in proper order. The pattern gets longer each time the player completes the pattern. If the player presses a wrong button, the game ends.

Variant: A nine-button version can add challenge to this game (but more than that would probably just be tedious.)

*/

#define SIMON_TITLE "Simon"

enum Simon_Mode {
    SimonMode_DisplayPattern,
    SimonMode_Playing,
    
    SimonMode_Count,
};

enum Simon_Button_Color {
    SimonButton_None,
    SimonButton_Yellow,
    SimonButton_Blue,
    SimonButton_Red,
    SimonButton_Green,
    
    SimonButton_Count,
};

struct Simon_State {
    Game_Mode game_mode;
    Game_Memory *memory;
    
    Simon_Mode mode;
    Simon_Button_Color pattern[6];
    Simon_Button_Color hovering_button;
    
    Vector2i dimensions;
    Vector2i mouse_position;
    
    u8 displaying_index;
    u8 displaying_count;
    u8 player_index;
    
    real32 flashing_t;
    real32 flashing_t_target;
    real32 flashing_dt;
};

internal Simon_Button_Color random_button();
internal void init_pattern(Simon_State *state);
internal void advance_pattern(Simon_State *state);

internal b32 is_inside_button(Vector2 offset, real32 pad, real32 button_size, Vector2i mouse_p, Simon_Button_Color button);
internal void update_hovering_button(Simon_State *state);

internal void draw_game_view(Simon_State *state);

internal void simon_menu_art(App_State *state, Vector2 min, Vector2 max);
internal void simon_game_restart(Simon_State *state);
internal void simon_game_update_and_render(Game_Memory *memory, Game_Input *input);
