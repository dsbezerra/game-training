internal loaded_font
load_font(char *fontpath, real32 size) {
    loaded_font result = {};
    
    u32 buffer_size = 1 << 20;
    u8 *bitmap = (u8 *) platform_alloc(BITMAP_SIZE * BITMAP_SIZE);
    
    file_contents font_contents = platform_read_entire_file(fontpath); 
    
    int ascent, descent, line_gap;
    stbtt_InitFont(&result.info, font_contents.contents, 0);
    stbtt_GetFontVMetrics(&result.info, &ascent, &descent, &line_gap);
    stbtt_BakeFontBitmap(font_contents.contents, 0, size, bitmap, BITMAP_SIZE, BITMAP_SIZE, 32, 224, result.cdata);
    result.scale = stbtt_ScaleForPixelHeight(&result.info, size);
    result.line_height = (real32) (ascent - descent + line_gap) * result.scale;
    
    
    glGenTextures(1, &result.texture);
    glBindTexture(GL_TEXTURE_2D, result.texture);
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, BITMAP_SIZE, BITMAP_SIZE, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    platform_free(font_contents.contents);
    platform_free(bitmap);
    
    return result;
}

internal real32
get_char_width(loaded_font *font, char c) {
    real32 result = 0.f;
    
    if (c >= 32 && c < 255) 
        result = font->cdata[c].xadvance;
    
    return result;
}


internal real32
get_text_width(loaded_font *font, char *text, int *line_count) {
    real32 result = 0.f;
    real32 w = 0.f;
    
    if (line_count) {
        *line_count = 0;
    }
    
    char *at = text;
    while (*at) {
        if (*at == '\n' && line_count) {
            if (w > result) {
                result = w;
            }
            w = .0f;
            (*line_count)++;
        } else if (*at >= 32 && *at < 255) {
            stbtt_bakedchar bc = font->cdata[*at];
            w += bc.xadvance;
        }
        *at++;
    }
    
    if (w > result) {
        result = w;
    }
    
    if (line_count) {
        (*line_count)++;
    } 
    
    return result;
}


internal real32
get_text_width(loaded_font *font, char *text) {
    return get_text_width(font, text, 0);
}