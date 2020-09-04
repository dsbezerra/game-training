internal loaded_font
load_font(char *fontpath, real32 size) {
    loaded_font result = {};
    
    u32 buffer_size = 1 << 20;
    u8 *buffer = (u8 *) platform_alloc(buffer_size);
    u8 *bitmap = (u8 *) platform_alloc(BITMAP_SIZE * BITMAP_SIZE);
    
    FILE *file = fopen(fontpath, "rb");
    fread(buffer, 1, buffer_size, file);
    fclose(file);
    
    int ascent, descent, line_gap;
    stbtt_InitFont(&result.info, buffer, 0);
    stbtt_GetFontVMetrics(&result.info, &ascent, &descent, &line_gap);
    stbtt_BakeFontBitmap(buffer, 0, size, bitmap, BITMAP_SIZE, BITMAP_SIZE, 32, 224, result.cdata);
    result.scale = stbtt_ScaleForPixelHeight(&result.info, size);
    result.line_height = (real32) (ascent - descent + line_gap) * result.scale;
    
    
    glGenTextures(1, &result.texture);
    glBindTexture(GL_TEXTURE_2D, result.texture);
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, BITMAP_SIZE, BITMAP_SIZE, 0, GL_ALPHA, GL_UNSIGNED_BYTE, bitmap);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    platform_free(buffer);
    platform_free(bitmap);
    
    return result;
}