/* date = November 14th 2020 11:40 pm */
/*

Training 8: Sokoban

Description: The player is in a level with objects that need to be pushed over goals. The objects can only be pushed, they can't be pulled. This game does require some effort to design levels for, but Sokoban levels have been designed by others and published on the web.

Variant: Add all sorts of level gimmicks: teleport tiles, conveyor belts, buttons that open doors/bridges, buttons that need an object left on them to keep a door open.

*/

#define SOKOBAN_TITLE "Sokoban"

#define SOKOBAN_WORLD_X 9
#define SOKOBAN_WORLD_Y 9

enum Sokoban_Entity_Kind {
    SokobanEntityKind_None,
    SokobanEntityKind_Block,
    SokobanEntityKind_Rock,
    SokobanEntityKind_Tree,
    SokobanEntityKind_Star,
    SokobanEntityKind_Sun,
    SokobanEntityKind_Count,
};

struct Sokoban_Entity {
    Sokoban_Entity_Kind kind;
    Vector3 position;
    Vector4 color;
};

struct Sokoban_Player {
    Vector3 position;
    Vector3 velocity;
    
    Triangle_Mesh mesh;
};

struct Sokoban_World {
    u32 x_count;
    u32 y_count;
    
    Sokoban_Entity *entities;
    u32 num_entities;
};

struct Sokoban_State {
    Game_Mode Game_Mode;
    
    Sokoban_Player player;
    Sokoban_World world;
    
    Triangle_Mesh block;
    Triangle_Mesh star;
    Triangle_Mesh plane;
    Triangle_Mesh sun;
    
    Playing_Sound *requiem;
    
    Camera cam;
    
    Vector2i dimensions;
    
    s8 menu_selected_item;
    b32 quit_was_selected;
};

internal Sokoban_Entity make_block(Vector3 position);
internal Sokoban_Entity make_star(Vector3 position);

internal void init_game(Sokoban_State *state);
internal void update_game(Sokoban_State *state, Game_Input *input);

internal void draw_game_view(Sokoban_State *state);

internal void sokoban_menu_art(Vector2 min, Vector2 max);
internal void sokoban_game_restart(Sokoban_State *state);
internal void sokoban_game_update_and_render(Game_Memory *memory, Game_Input *input);
internal void sokoban_game_free(Game_Memory *memory);