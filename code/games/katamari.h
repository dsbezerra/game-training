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

enum katamari_entity_kind {
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

enum katamari_movement_pattern {
    KatamariMovementPattern_None,
    KatamariMovementPattern_ZigZag,
    KatamariMovementPattern_LeftRight,
    KatamariMovementPattern_TopBottom,
    KatamariMovementPattern_FollowPlayer,
    KatamariMovementPattern_Count,
};

struct katamari_entity {
    katamari_entity_kind kind;
    katamari_movement_pattern movement_pattern;
    v2 position;
    v2 direction;
    v2 half_size;
    b32 alive;
    
    real32 speed;
    real32 zigzag_t;
    real32 zigzag_target;
    
    real32 stopped_t;
    real32 stopped_t_target;
};

struct katamari_assets {
    GLuint grass[3];
    GLuint squirrel[2];
};

struct katamari_state {
    v2i dimensions;
    
    game_mode game_mode;
    
    katamari_assets assets;
    
    // NOTE(diego): Slots above 32 are reserved to squirrels.
    katamari_entity entities[64];
    
    real32 spawn_t;
    real32 spawn_t_target;
    
    u8 health;
    
    s8 menu_selected_item;
    b32 quit_was_selected;
};

internal katamari_entity make_entity(katamari_entity_kind kind);
internal katamari_entity make_grass(katamari_entity_kind kind, real32 x, real32 y);
internal katamari_entity* find_first_entity(katamari_state *state, katamari_entity_kind kind);

internal void spawn_squirrel(katamari_state *state, u32 count);
internal b32 squirrel_collided(katamari_state *state, katamari_entity *squirrel, u8 wall_number);
internal void squirrel_handle_collision(katamari_state *state, katamari_entity *player, katamari_entity *squirrel);

internal void init_game(katamari_state *state);
internal void init_textures(katamari_assets *assets);
internal void free_textures(katamari_assets *assets);
internal void update_game(katamari_state *state, game_input *input);

internal void draw_game_view(katamari_state *state);
internal void draw_player(katamari_state *state, katamari_entity *entity);
internal void draw_squirrel(katamari_state *state, katamari_entity *entity);
internal void draw_grass(katamari_state *state, katamari_entity *entity);
internal void draw_entity(katamari_state *state, katamari_entity *entity);
internal void draw_hud(katamari_state *state);

internal void katamari_menu_art(v2 min, v2 max);
internal void katamari_game_restart(katamari_state *state);
internal void katamari_game_update_and_render(game_memory *memory, game_input *input);
internal void katamari_game_free(game_memory *memory);