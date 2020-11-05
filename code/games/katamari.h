/* date = November 1st 2020 8:54 pm */
/*

Training 7: Katamari Damacy

Description: The original Katamari Damacy game was in a 3d world, but a 2d version is also easy to implement. The player controls a small object in a world of different-sized objects. Touching the smaller objects grows the player, touching the larger objects damages or shrinks the player. The player wins when they reach a certain size.

*/

#define KATAMARI_DAMACY_TITLE "Katamari Damacy"
#define KATAMARI_PLAYER_SIZE 24.f

enum katamari_entity_kind {
    KatamariEntity_None,
    
    KatamariEntity_Player,
    KatamariEntity_Squirrel,
    KatamariEntity_Grass1,
    KatamariEntity_Grass2,
    KatamariEntity_Grass3,
    
    KatamariEntity_Count,
};

struct katamari_entity {
    katamari_entity_kind kind;
    v2 position;
    v2 half_size;
};


struct katamari_assets {
    GLuint grass[3];
};

struct katamari_state {
    v2i dimensions;
    
    game_mode game_mode;
    
    katamari_assets assets;
    
    katamari_entity entities[256];
    
    u8 health;
    
    s8 menu_selected_item;
    b32 quit_was_selected;
};

internal katamari_entity make_entity(katamari_entity_kind kind);
internal katamari_entity make_grass(katamari_entity_kind kind, real32 x, real32 y);
internal katamari_entity* find_first_entity(katamari_state *state, katamari_entity_kind kind);

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