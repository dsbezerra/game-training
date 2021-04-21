/* date = April 20th 2021 8:23 pm */

#define UPLOAD_SPRITESHEET 0x01

enum Sprite_Sheet_Token {
    SpriteSheetToken_Name,
    SpriteSheetToken_X,
    SpriteSheetToken_Y,
    SpriteSheetToken_Width,
    SpriteSheetToken_Height,
    SpriteSheetToken_Count,
};

struct Sprite {
    s32 index; // NOTE(diego): This is >= 0 and <= num_sprites if spritesheet is being used
    
    u32 texture_id;
    u32 x; // top_x
    u32 y; // top_x
    u32 width;
    u32 height;
    
    char *name;
};

struct Spritesheet {
    char *filepath;
    char *infopath;
    
    u32 num_sprites;
    Sprite *sprites;
    
    s32 width;
    s32 height;
};

internal void consume_sprite(Sprite *sprite, char *line);
internal u32 consume_sprite(Spritesheet *spritesheet, char *line, u32 sprite_index);

internal Sprite * load_sprite(char *filepath);
internal Spritesheet * load_spritesheet(char *filepath);

internal void init_spritesheet(Spritesheet *spritesheet, u32 flags = 0);
internal void upload_spritesheet(Spritesheet *spritesheet);
