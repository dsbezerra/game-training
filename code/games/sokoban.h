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
    SokobanEntityKind_Player,
    SokobanEntityKind_Block,
    SokobanEntityKind_Star,
    SokobanEntityKind_Sun,
    SokobanEntityKind_Goal,
    SokobanEntityKind_Count,
};

struct Sokoban_World_Position {
    s32 x;
    s32 y;
};

struct Sokoban_Entity {
    u32 id;
    
    Sokoban_Entity_Kind kind;
    Sokoban_World_Position world_position;
    
    Vector3 position;
    Vector4 color;
    
    // NOTE(diego): Used for SokobanEntityKind_Goal entities to indicate whether
    // a SokobanEntityKind_Star is currently in the same position or not.
    s32 entity_activated_index;
};

struct Sokoban_Player {
    s32 entity_index;
};

struct Sokoban_State;

struct Sokoban_World {
    Sokoban_State *state;
    
    u32 x_count;
    u32 y_count;
    
    Sokoban_Entity *entities;
    u32 num_entities;
};

#define SOKOBAN_CHANGE_FROM 0
#define SOKOBAN_CHANGE_TO 1
enum Sokoban_Change_Type {
    SokobanChange_None,
    
    SokobanChange_entity_location,
    
    SokobanChange_Count,
};

struct Sokoban_Entity_Location_Change {
    s32 entity_index;
    Sokoban_World_Position change_to_value;
    Sokoban_World_Position change_from_value;
};

struct Sokoban_Change {
    Sokoban_Change *next;
    Sokoban_Change *prev;
    
    Sokoban_Change_Type type;
    union {
        Sokoban_Entity_Location_Change entity_location;
    };
};

struct Sokoban_State {
    Game_Mode Game_Mode;
    
    Sokoban_World *world;
    Sokoban_Player player;
    
    Sokoban_Change undo_sentinel;
    Sokoban_Change redo_sentinel;
    
    u32 current_level;
    
    Triangle_Mesh block;
    Triangle_Mesh star;
    Triangle_Mesh plane;
    Triangle_Mesh sun;
    Triangle_Mesh arrow;
    Triangle_Mesh goal;
    
    Playing_Sound *requiem;
    
    Camera cam;
    Vector3 lock_position;
    
    real32 lock_pitch;
    real32 lock_yaw;
    real32 cam_animation_rate;
    
    Vector2i dimensions;
    
    s8 menu_selected_item;
    b32 quit_was_selected;
};

internal Sokoban_Entity make_block(Vector3 position);
internal Sokoban_Entity make_star(Vector3 position);

internal void adjust_camera_to_level(Sokoban_State *state, b32 animate);
internal void init_game(Sokoban_State *state);
internal void update_game(Sokoban_State *state, Game_Input *input);


internal b32 are_same_world_position(Sokoban_World_Position *a, Sokoban_World_Position *b);
internal void ensure_world_position_is_valid(Sokoban_World *world, Sokoban_World_Position *world_position);

internal Vector3 to_visual_position(Sokoban_World *world, Sokoban_World_Position world_position, Sokoban_Entity_Kind kind = SokobanEntityKind_None);

internal b32 is_empty(Sokoban_Change *list);
internal b32 undo_available(Sokoban_State *state);
internal b32 redo_available(Sokoban_State *state);
internal void link(Sokoban_Change *prev, Sokoban_Change *change);
internal void sentinelize(Sokoban_Change *change);
internal void release_sentinel(Sokoban_Change *change);
internal void unlink(Sokoban_Change *change);
internal Sokoban_Change * pop_first(Sokoban_Change *prev);
internal void push_first(Sokoban_Change *prev, Sokoban_Change *pushed);
internal void undo(Sokoban_State *state);
internal void redo(Sokoban_State *state);
internal void apply_change(Sokoban_State *state, Sokoban_Change *change, int type);

internal void recorded_change_entity_location(Sokoban_State *state, s32 entity_index, Sokoban_World_Position *old_position, Sokoban_World_Position *new_position);

internal void change_entity_location(Sokoban_World *world, s32 entity_index, Sokoban_World_Position *old_position, Sokoban_World_Position *new_position);

internal s32 is_position_occupied(Sokoban_World *world, Sokoban_World_Position test_position);
internal b32 is_pushable(Sokoban_Entity_Kind kind);
internal b32 push_entity(Sokoban_World *world, s32 entity_index, Sokoban_World_Position diff);

internal void set_activate_goal_state(Sokoban_Entity *entity, s32 activator_index);

internal void release_current_level(Sokoban_State *state);
internal void previous_level(Sokoban_State *state);
internal void next_level(Sokoban_State *state);
internal Sokoban_World * load_level(Sokoban_State *state, char *levelname);

internal void draw_game_view(Sokoban_State *state);

internal void sokoban_menu_art(Vector2 min, Vector2 max);
internal void sokoban_game_restart(Sokoban_State *state);
internal void sokoban_game_update_and_render(Game_Memory *memory, Game_Input *input);
internal void sokoban_game_free(Game_Memory *memory);
