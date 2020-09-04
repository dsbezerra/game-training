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
    dodger_world world;
    dodger_player player;
    dodger_bad_guy bad_guys[64];
    u64 score;
    u64 top_score;
};


internal void dodger_game_update_and_render(game_memory *memory, game_input *input);

internal void init_world(dodger_state *state);
internal void init_player(dodger_state *state);
internal void init_bad_guy(dodger_state *state, dodger_bad_guy *bad_guy);
internal void init_bad_guys(dodger_state *state);

internal void update_player(dodger_player *player);
internal void update_bad_guy(dodger_state *state, dodger_bad_guy *bad_guy);

internal void draw_player(dodger_player *player);
internal void draw_bad_guy(dodger_bad_guy *bad_guy);
