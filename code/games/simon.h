/* date = September 19th 2020 9:12 pm */
/*
 
Training 4: Simon

Description: Four colored buttons light up in a specific pattern. After displaying the pattern, the player must repeat the pattern by clicking the buttons in proper order. The pattern gets longer each time the player completes the pattern. If the player presses a wrong button, the game ends.

Variant: A nine-button version can add challenge to this game (but more than that would probably just be tedious.)

*/

#define SIMON_TITLE "Simon"

enum simon_mode {
    SimonMode_DisplayPattern,
    SimonMode_Playing,
    
    SimonMode_Count,
};

enum simon_button_color {
    SimonButton_None,
    SimonButton_Yellow,
    SimonButton_Blue,
    SimonButton_Red,
    SimonButton_Green,
    
    SimonButton_Count,
};

struct simon_state {
    v2i dimensions;
    v2i mouse_position;
    
    simon_mode mode;
    simon_button_color pattern[6];
    
    
    u8 displaying_index;
    u8 displaying_count;
    u8 player_index;
    
    simon_button_color hovering_button;
    
    real32 flashing_t;
    real32 flashing_t_target;
    real32 flashing_dt;
    
    game_mode game_mode;
    s8 menu_selected_item;
    b32 quit_was_selected;
};

internal simon_button_color random_button();
internal void init_pattern(simon_state *state);
internal void advance_pattern(simon_state *state);

internal b32 is_inside_button(v2 offset, real32 pad, real32 button_size, v2i mouse_p, simon_button_color button);
internal void update_hovering_button(simon_state *state);

internal void draw_game_view(simon_state *state);

internal void simon_menu_art(v2 min, v2 max);
internal void simon_game_restart(simon_state *state);
internal void simon_game_update_and_render(game_memory *memory, game_input *input);
