/*

 Training 1: Dodger
 http://inventwithpython.com/blog/2012/02/20/i-need-practice-programming-49-ideas-for-game-clones-to-code/

Description: Several bad guys fall from the top of the screen, and the user must avoid them. The player can be controlled with the arrow keys or more directly with the mouse. The longer the player lasts without being hit, the higher the score.

Variations: Have enemies fall at different rates and be different sizes. Have enemies fall from more than one side of the game. Have power up pickups that grant invulnerability for a while, slow down bad guys, give the player a temporary "reverse bad guys" power, etc.

 TODO(diego): 

- Create images for player and bad guys.

*/

#define DODGER_TITLE "Dodger"

struct Dodger_Assets {
    Loaded_Font primary_font;
};

struct Dodger_World {
    Vector2i dimensions;
};

struct Dodger_Player {
    Vector2 position;
    Vector2 size;
};

struct Dodger_Bad_Guy {
    Vector2 position;
    Vector2 size;
};

struct Dodger_State {
    Game_Mode game_mode;
    Game_Memory *memory;
    
    Dodger_Assets assets;
    
    Dodger_World world;
    Dodger_Player player;
    Dodger_Bad_Guy bad_guys[32];
    
    u64 score;
    u64 top_score;
};

internal void dodger_game_restart(Dodger_State *state);

internal void dodger_menu_art(App_State *state, Vector2 min, Vector2 max);
internal void dodger_game_update_and_render(Game_Memory *memory, Game_Input *input);

internal void init_world(Dodger_State *state);
internal void init_player(Dodger_State *state);
internal void init_bad_guy(Dodger_State *state, Dodger_Bad_Guy *bad_guy);
internal void init_bad_guys(Dodger_State *state);

internal void update_player(Dodger_State *state, Game_Input *input, real32 dt);
internal void update_bad_guy(Dodger_State *state, Dodger_Bad_Guy *bad_guy, real32 dt);

internal void draw_player(Dodger_Player *player);
internal void draw_bad_guy(Dodger_Bad_Guy *bad_guy);

internal void draw_game_view(Dodger_State *state);
internal void draw_game_over(Dodger_State *state);
