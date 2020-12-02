
struct shader_source {
    char vertex[1 << 10];
    char fragment[1 << 10];
};

struct shader {
    GLuint program;
    
    GLint model_loc;
    GLint view_loc;
    GLint projection_loc;
    GLint texture_loc;
    
    GLint position_loc;
    GLint color_loc;
    GLint uv_loc;
    GLint normal_loc;
    
    GLint light_color_loc;
};

internal shader_source parse_shader(char *filepath);

internal void check_compile_error(int shader);
internal void check_linking_error(int program);
internal int compile_shader(int type, char *source);
internal int link_shaders(int n, ...);
internal void delete_shader(int shader);
internal void delete_shaders(int n, ...);
internal void init_shaders();

internal void set_float4(v4 values);