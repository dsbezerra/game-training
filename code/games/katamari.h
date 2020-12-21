/* date = November 1st 2020 8:54 pm */
/*

Training 7: Katamari Damacy

Description: The original Katamari Damacy game was in a 3d world, but a 2d version is also easy to implement. The player controls a small object in a world of different-sized objects. Touching the smaller objects grows the player, touching the larger objects damages or shrinks the player. The player wins when they reach a certain size.

*/

#define KATAMARI_DAMACY_TITLE "Katamari Damacy"

#define KATAMARI_SQUIRREL_BASE_INDEX 32

#define KATAMARI_PLAYER_SIZE 32.f
#define KATAMARI_SQUIRREL_MAX_SIZE 45.f

#define KATAMARI_SQUIRREL_SPEED 50.f

enum Katamari_Entity_Kind {
    KatamariEntity_None,
    
    KatamariEntity_Player,
    KatamariEntity_Squirrel1,
    KatamariEntity_Squirrel2,
    KatamariEntity_Squirrel3,
    KatamariEntity_Grass1,
    KatamariEntity_Grass2,
    KatamariEntity_Grass3,
    
    KatamariEntity_Count,
};

enum Katamari_Movement_Pattern {
    KatamariMovementPattern_None,
    KatamariMovementPattern_ZigZag,
    KatamariMovementPattern_LeftRight,
    KatamariMovementPattern_TopBottom,
    KatamariMovementPattern_FollowPlayer,
    KatamariMovementPattern_Count,
};

struct Katamari_Entity {
    Katamari_Entity_Kind kind;
    Katamari_Movement_Pattern movement_pattern;
    Vector2 position;
    Vector2 velocity;
    Vector2 direction;
    Vector2 half_size;
    b32 alive;
    
    real32 speed;
    real32 zigzag_t;
    real32 zigzag_target;
    
    real32 stopped_t;
    real32 stopped_t_target;
};

struct Katamari_Assets {
    GLuint grass[3];
    GLuint squirrel[2];
};

struct Katamari_State {
    Vector2i dimensions;
    
    Game_Mode Game_Mode;
    
    Katamari_Assets assets;
    
    // NOTE(diego): Slots above 32 are reserved to squirrels.
    Katamari_Entity entities[64];
    
    real32 spawn_t;
    real32 spawn_t_target;
    
    u8 health;
    
    s8 menu_selected_item;
    b32 quit_was_selected;
};

internal Katamari_Entity make_entity(Katamari_Entity_Kind kind);
internal Katamari_Entity make_grass(Katamari_Entity_Kind kind, real32 x, real32 y);
internal Katamari_Entity* find_first_entity(Katamari_State *state, Katamari_Entity_Kind kind);

internal void spawn_squirrel(Katamari_State *state, u32 count);
internal b32 squirrel_collided(Katamari_State *state, Katamari_Entity *squirrel, u8 wall_number);
internal void squirrel_handle_collision(Katamari_State *state, Katamari_Entity *player, Katamari_Entity *squirrel);

internal void init_game(Katamari_State *state);
internal void init_textures(Katamari_Assets *assets);
internal void free_textures(Katamari_Assets *assets);
internal void update_game(Katamari_State *state, Game_Input *input);

internal void draw_game_view(Katamari_State *state);
internal void draw_player(Katamari_State *state, Katamari_Entity *entity);
internal void draw_squirrel(Katamari_State *state, Katamari_Entity *entity);
internal void draw_grass(Katamari_State *state, Katamari_Entity *entity);
internal void draw_entity(Katamari_State *state, Katamari_Entity *entity);
internal void draw_hud(Katamari_State *state);

internal void katamari_menu_art(App_State *state, Vector2 min, Vector2 max);
internal void katamari_game_restart(Katamari_State *state);
internal void katamari_game_update_and_render(Game_Memory *memory, Game_Input *input);
internal void katamari_game_free(Game_Memory *memory);