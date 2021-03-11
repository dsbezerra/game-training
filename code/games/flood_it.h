/* date = March 7th 2021 2:20 pm */
/*

Training 10: Flood It

Description: A grid of six colors of tiles starts off randomly. The player can do a "flood fill" on the top left tile, changing the color of any adjacent tiles of thesame color. The player wins if they are able to make the entire board a single color within a certain number of moves.

Variants: Power ups gained when a certain tile is changed.

*/

#define FLOOD_IT_TITLE "Flood It"

#define FLOOD_IT_GRID_SIZE 17

enum Flood_It_Tile_Kind {
    FloodItTileKind_Red,
    FloodItTileKind_Green,
    FloodItTileKind_Blue,
    FloodItTileKind_Yellow,
    FloodItTileKind_Orange,
    FloodItTileKind_Pink,
    
    FloodItTileKind_Count,
};

struct Flood_It_Assets {
    Loaded_Font debug;
};

struct Flood_It_State;

struct Flood_It_Tile {
    Flood_It_Tile_Kind kind;
    
    u32 x;
    u32 y;
    
    Vector4 color;
};

struct Flood_It_Color {
    Flood_It_Tile_Kind kind;
    
    real32 x;
    real32 y;
    real32 w;
    real32 h;
    
    Vector4 color;
    
    b32 hover;
};

struct Flood_It_Grid {
    Flood_It_Tile tiles[FLOOD_IT_GRID_SIZE][FLOOD_IT_GRID_SIZE];
};

struct Flood_It_State {
    Game_Mode game_mode;
    
    Flood_It_Assets assets;
    Flood_It_Grid grid;
    Flood_It_Color colors[FloodItTileKind_Count];
    
    Flood_It_Color *hovered_color;
    
    Vector2i mouse_position;
    Vector2i dimensions;
    
    s8 menu_selected_item;
    b32 quit_was_selected;
};

internal void init_game(Flood_It_State *state);
internal void update_game(Flood_It_State *state, Game_Input *input);
internal void update_colors(Flood_It_State *state);

internal void flood_fill(Flood_It_State *state);
internal void flood_tile(Flood_It_State *state, Flood_It_Color *color, u32 x, u32 y, Flood_It_Tile_Kind first, b32 first_tile);
internal void generate_grid(Flood_It_State *state);
internal Flood_It_Tile make_random_tile(u32 x, u32 y);
internal Vector4 get_color_for_tile(Flood_It_Tile_Kind kind);
internal void set_color(Flood_It_Color *color, Flood_It_Tile *tile);

internal b32 is_mouse_over(Flood_It_State *state, Flood_It_Color *color);

internal void draw_colors(Flood_It_State *state);
internal void draw_grid(Flood_It_State *state);
internal void draw_game_view(Flood_It_State *state);
internal void draw_hud(Flood_It_State *state);

internal void flood_it_menu_art(App_State *state, Vector2 min, Vector2 max);
internal void flood_it_game_restart(Flood_It_State *state);
internal void flood_it_game_update_and_render(Game_Memory *memory, Game_Input *input);
internal void flood_it_game_free(Game_Memory *memory);
