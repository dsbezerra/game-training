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
    //KatamariEntity_Grass,
    
    KatamariEntity_Count,
};

struct katamari_entity {
    katamari_entity_kind kind;
    v2 position;
    v2 half_size;
};


struct katamari_assets {
    GLuint grass;
};

struct katamari_state {
    v2i dimensions;
    
    game_mode game_mode;
    
    katamari_assets assets;
    
    katamari_entity entities[256];
    
    s8 menu_selected_item;
    b32 quit_was_selected;
};

internal katamari_entity make_entity(katamari_entity_kind kind);

internal void init_game(katamari_state *state);
internal void update_game(katamari_state *state, game_input *input);

internal void draw_game_view(katamari_state *state);
internal void draw_player(katamari_state *state, katamari_entity *entity);
internal void draw_squirrel(katamari_state *state, katamari_entity *entity);
internal void draw_entity(katamari_state *state, katamari_entity *entity);

internal void katamari_menu_art(v2 min, v2 max);
internal void katamari_game_restart(katamari_state *state);
internal void katamari_game_update_and_render(game_memory *memory, game_input *input);
internal void katamari_game_free(game_memory *memory);