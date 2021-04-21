internal void
consume_sprite(Sprite *sprite, char *line) {
    Sprite_Sheet_Token current_token = SpriteSheetToken_Name;
    Break_String_Result r = break_by_tok(line, ',');
    if (!r.lhs) {
        return;
    }
    
    while (current_token < SpriteSheetToken_Count) {
        switch (current_token) {
            case SpriteSheetToken_Name: {
                sprite->name = copy_string(r.lhs); current_token = SpriteSheetToken_X;
            } break;
            
            case SpriteSheetToken_X: {
                sprite->x = atoi(r.lhs); current_token = SpriteSheetToken_Y;
            } break;
            
            case SpriteSheetToken_Y: {
                sprite->y = atoi(r.lhs); current_token = SpriteSheetToken_Width;
            } break;
            
            case SpriteSheetToken_Width: {
                sprite->width = atoi(r.lhs); current_token = SpriteSheetToken_Height;
            } break;
            
            case SpriteSheetToken_Height: {
                sprite->height = atoi(r.lhs); current_token = SpriteSheetToken_Count;
            } break;
            
            default: invalid_code_path; break;
        }
        r = break_by_tok(r.rhs, ',');
        if (!r.lhs) break;
    }
    
    sprite->index = -1;
}

internal u32
consume_sprite(Spritesheet *spritesheet, char *line, u32 sprite_index) {
    Sprite *sprite = &spritesheet->sprites[sprite_index];
    consume_sprite(sprite, line);
    
    return sprite->index = sprite_index + 1;
}


internal Sprite *
load_sprite(char *filepath) {
    Sprite *result = 0;
    // TODO(diego): Implement.
    return result;
}

internal Spritesheet *
load_spritesheet(char *infopath) {
    assert(string_length(infopath) != 0);
    assert(string_ends_with(infopath, ".txt"));
    
    File_Contents sprite_file = platform_read_entire_file(infopath);
    assert(sprite_file.file_size > 0);
    
    Text_File_Handler handler = {};
    init_handler(&handler, sprite_file.contents);
    
    assert(handler.num_lines > 0);
    
    // sprites.txt file contains a series of sprites like:
    //
    // gem1,0,0,64,64
    // name,top_x,top_y,width,height
    //
    // where gem1 is the name, 0
    Spritesheet *result = (Spritesheet *) platform_alloc(sizeof(Spritesheet));
    result->infopath = copy_string(infopath);
    result->num_sprites = handler.num_lines; // NOTE(diego): We assume each line is a sprite info.
    result->sprites = (Sprite *) platform_alloc(result->num_sprites * sizeof(Sprite));
    
    u32 consumed = 0;
    while (1) {
        char *line = consume_next_line(&handler);
        if (!line) break;
        
        consumed = consume_sprite(result, line, consumed);
    }
    
    platform_free(handler.data);
    
    return result;
}

internal void
init_spritesheet(Spritesheet *spritesheet, u32 flags) {
    assert(spritesheet);
    
    if (!spritesheet->filepath) {
        assert(spritesheet->infopath);
        assert(string_ends_with(spritesheet->infopath, ".txt"));
        
        u32 len = string_length(spritesheet->infopath);
        int t = find_character_from_right(spritesheet->infopath, '/', len);
        
        Break_String_Result r = break_at_index(spritesheet->infopath, t + 1);
        
        char *s = "spritesheet.png";
        spritesheet->filepath = concat(r.lhs, s, t + 1, string_length(s));
    }
    
    // Assume we have a filepath
    if (flags & UPLOAD_SPRITESHEET) {
        upload_spritesheet(spritesheet);
    }
}

internal void
upload_spritesheet(Spritesheet *spritesheet) {
    GLuint texture_id = load_texture(spritesheet->filepath, spritesheet->width, spritesheet->height);
    if (texture_id) {
        // Set same texture id for all sprites
        for (u32 sprite_index = 0; sprite_index < spritesheet->num_sprites; ++sprite_index) {
            Sprite *sprite = &spritesheet->sprites[sprite_index];
            sprite->texture_id = texture_id;
        }
    }
    spritesheet->texture_id = texture_id;
}