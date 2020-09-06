enum dodger_mode {
    DodgerMode_Menu,
    DodgerMode_Playing,
    DodgerMode_GameOver,
};

struct dodger_assets {
    loaded_font primary_font;
    loaded_font menu_title_font;
    loaded_font menu_item_font;
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
    dodger_mode game_mode;
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

internal void update_player(dodger_state *state);
internal void update_bad_guy(dodger_state *state, dodger_bad_guy *bad_guy);

internal void draw_player(dodger_player *player);
internal void draw_bad_guy(dodger_bad_guy *bad_guy);

internal void draw_game_view(dodger_state *state);
internal void draw_game_over(dodger_state *state);
