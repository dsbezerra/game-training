/* date = November 14th 2020 11:40 pm */
/*

Training 8: Sokoban

Description: The player is in a level with objects that need to be pushed over goals. The objects can only be pushed, they can't be pulled. This game does require some effort to design levels for, but Sokoban levels have been designed by others and published on the web.

Variant: Add all sorts of level gimmicks: teleport tiles, conveyor belts, buttons that open doors/bridges, buttons that need an object left on them to keep a door open.

*/

#define SOKOBAN_TITLE "Sokoban"

enum Sokoban_Entity_Kind {
    SokobanEntityKind_None,
    SokobanEntityKind_Tile,
    SokobanEntityKind_Light,
    SokobanEntityKind_Count,
};

struct Sokoban_Entity {
    Sokoban_Entity_Kind kind;
    Vector3 position;
    Vector4 color;
};

struct Sokoban_Assets {
    GLint none;
    GLint container;
    GLint container_specular;
};

struct Sokoban_Player {
    Vector3 position;
    Vector3 velocity;
    
    Triangle_Mesh mesh;
};

struct Sokoban_State {
    Game_Mode Game_Mode;
    
    Sokoban_Assets assets;
    Sokoban_Player player;
    Sokoban_Entity entities[16 * 16];

    Camera cam;
    
    Vector2i dimensions;

    s8 menu_selected_item;
    b32 quit_was_selected;
};

internal void init_game(Sokoban_State *state);
internal void update_game(Sokoban_State *state, Game_Input *input);

internal void draw_game_view(Sokoban_State *state);

internal void sokoban_menu_art(Vector2 min, Vector2 max);
internal void sokoban_game_restart(Sokoban_State *state);
internal void sokoban_game_update_and_render(Game_Memory *memory, Game_Input *input);
