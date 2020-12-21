#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"

#define BITMAP_SIZE 512

struct Loaded_Font {
    GLuint texture;
    stbtt_bakedchar cdata[224];
    stbtt_fontinfo info;
    real32 scale;
    real32 line_height;
};

internal Loaded_Font load_font(char *fontpath, real32 size);