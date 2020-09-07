
global_variable shader global_shader;

internal shader_source
parse_shader(char *filepath) {
    shader_source result = {};
    
    file_contents file = plataform_read_entire_file(filepath);
    
    char *at = (char *) file.contents;
    u8 type = 0;
    
    size_t vertex_header_length = string_length("#shader vertex");
    size_t fragment_header_length = string_length("#shader fragment");
    
    u8 *vertex = (u8*) result.vertex;
    u8 *fragment = (u8*) result.fragment;
    
    while (*at) {
        if (*at == '#') {
            if (string_starts_with("#shader vertex", at)) {
                type = 1; at += vertex_header_length + 1;
                continue;
            } else if (string_starts_with("#shader fragment", at)) {
                type = 2; at += fragment_header_length + 1;
                continue;
            }
        }
        
        
        switch (type) {
            case 1: {
                *vertex++ = *at++;
            } break;
            case 2: {
                *fragment++ = *at++;
            } break;
            
            default: break;
        }
    }
    
    platform_free(file.contents);
    
    return result;
}

internal void
check_compile_error(int shader) {
    int success;
    char log[512];
    open_gl->glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        open_gl->glGetShaderInfoLog(shader, 512, 0, log);
        OutputDebugString("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n");
        OutputDebugString(log);
        OutputDebugString("\n");
    }
}

internal void
check_linking_error(int program) {
    int success;
    open_gl->glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLsizei ignored;
        char program_errors[4096];
        open_gl->glGetProgramInfoLog(program, sizeof(program_errors), &ignored, program_errors);
        
        OutputDebugString("ERROR::SHADER::PROGRAM::LINKING_FAILED\n");
        if (program_errors[0] != 0) {
            OutputDebugString(program_errors);
            OutputDebugString("\n");
        }
        OutputDebugString("\n");
    }
}

internal int
compile_shader(int type, char *source) {
    int shader = open_gl->glCreateShader(type);
    open_gl->glShaderSource(shader, 1, &source, 0);
    open_gl->glCompileShader(shader);
    check_compile_error(shader);
    return shader;
}

internal int
link_shaders(int n, ...) {
    va_list ap;
    va_start(ap, n);
    int program = open_gl->glCreateProgram();
    int *shaders = new int[n];
    int *at = shaders;
    for (int i = 0; i < n; i++) {
        int shader = va_arg(ap, int);
        *at++ = shader;
        open_gl->glAttachShader(program, shader);
    }
    va_end(ap);
    open_gl->glLinkProgram(program);
    check_linking_error(program);
    
    while (*at) {
        open_gl->glDetachShader(program, *at);
        *at++;
    }
    
    return program;
}

internal void
delete_shader(int shader) {
    open_gl->glDeleteShader(shader);
}

internal void
delete_shaders(int n, ...) {
    va_list ap;
    va_start(ap, n);
    for (int i = 0; i < n; i++) {
        int shader = va_arg(ap, int);
        delete_shader(shader);
    }
    va_end(ap);
}

internal void
init_shaders() {
    shader_source source = parse_shader("./data/shaders/argb_texture.glsl");
    shader result = {};
    
    int vertex_shader = compile_shader(GL_VERTEX_SHADER, source.vertex);
    int fragment_shader = compile_shader(GL_FRAGMENT_SHADER, source.fragment);
    int program = link_shaders(2, vertex_shader, fragment_shader);
    delete_shaders(2, vertex_shader, fragment_shader);
    
    result.program = program;
    result.projection_loc = open_gl->glGetUniformLocation(program, "projection");
    result.view_loc = open_gl->glGetUniformLocation(program, "view");
    result.texture_loc = open_gl->glGetUniformLocation(program, "ftex");
    result.position_loc = 0;
    result.color_loc    = 1;
    result.uv_loc       = 2;
    result.z_index_loc  = 3;
    
    global_shader = result;
}