/*

 Training 1: Dodger
 http://inventwithpython.com/blog/2012/02/20/i-need-practice-programming-49-ideas-for-game-clones-to-code/

Description: Several bad guys fall from the top of the screen, and the user must avoid them. The player can be controlled with the arrow keys or more directly with the mouse. The longer the player lasts without being hit, the higher the score.

Variations: Have enemies fall at different rates and be different sizes. Have enemies fall from more than one side of the game. Have power up pickups that grant invulnerability for a while, slow down bad guys, give the player a temporary "reverse bad guys" power, etc.

 TODO(diego): 

- Create images for player and bad guys.

*/

#define DODGER_TITLE "Dodger"

struct dodger_assets {
    loaded_font primary_font;
};

struct dodger_world {
    v2i dimensions;
};

struct dodger_player {
    v2 position;
    v2 size;
};

struct dodger_bad_guy {
    v2 position;
    v2 size;
};

struct dodger_state {
    game_mode game_mode;
    dodger_assets assets;
    dodger_world world;
    dodger_player player;
    dodger_bad_guy bad_guys[32];
    
    u64 score;
    u64 top_score;
    
    s8 menu_selected_item;
    b32 quit_was_selected;
};

internal void dodger_game_restart(dodger_state *state);

internal void dodger_menu_art(v2i min, v2i max);
internal void dodger_game_update_and_render(game_memory *memory, game_input *input);

internal void init_world(dodger_state *state);
internal void init_player(dodger_state *state);
internal void init_bad_guy(dodger_state *state, dodger_bad_guy *bad_guy);
internal void init_bad_guys(dodger_state *state);

internal void update_player(dodger_state *state, game_input *input, real32 dt);
internal void update_bad_guy(dodger_state *state, dodger_bad_guy *bad_guy, real32 dt);

internal void draw_player(dodger_player *player);
internal void draw_bad_guy(dodger_bad_guy *bad_guy);

internal void draw_game_view(dodger_state *state);
internal void draw_game_over(dodger_state *state);
