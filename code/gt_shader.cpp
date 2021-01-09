global_variable Shader global_shader;
global_variable Shader global_basic_3d_shader;
global_variable Shader global_basic_3d_light_shader;
global_variable Shader global_screen_shader;

internal void
init_shader_catalog() {
    StringArray *extensions = make_string_array(1);
    array_add_if_unique(extensions, "glsl");
    
    shader_catalog.base.extensions = extensions;
    shader_catalog.base.kind = CatalogKind_Shader;
    shader_catalog.base.my_name = "Shader Catalog";
    shader_catalog.base.size = SHADER_CATALOG_SIZE;
    shader_catalog.data = (Shader *) platform_alloc(shader_catalog.base.size * sizeof(Shader));
}

internal void
add_shader(Shader *shader) {
    if (!shader_catalog.data) {
        init_shader_catalog();
    }
    for (u32 i = 0; i < shader_catalog.base.size; ++i) {
        Shader *s = &shader_catalog.data[i];
        if (!s->loaded) {
            *s = *shader;
            break;
        }
    }
}

internal Shader *
find_shader(char *name) {
    if (!name) return 0;
    if (!shader_catalog.data) return 0;
    if (shader_catalog.base.size == 0) return 0;
    
    for (u32 i = 0; i < shader_catalog.base.size; ++i) {
        Shader *shader = &shader_catalog.data[i];
        if (!shader->short_name || !shader->loaded) continue;
        if (strings_are_equal(shader->short_name, name)) {
            return shader;
        }
    }
    return 0;
}

internal Shader_Source
parse_shader(char *filepath) {
    Shader_Source result = {};
    
    File_Contents file = platform_read_entire_file(filepath);
    
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

internal Shader
load_shader(char *filepath) {
    Shader_Source source = parse_shader(filepath);
    Shader result = {};
    
    int vertex_shader = compile_shader(GL_VERTEX_SHADER, source.vertex);
    int fragment_shader = compile_shader(GL_FRAGMENT_SHADER, source.fragment);
    int program = link_shaders(2, vertex_shader, fragment_shader);
    delete_shaders(2, vertex_shader, fragment_shader);
    
    result.program = program;
    result.model_loc = open_gl->glGetUniformLocation(program, "model");
    result.view_loc = open_gl->glGetUniformLocation(program, "view");
    result.projection_loc = open_gl->glGetUniformLocation(program, "projection");
    result.texture_loc = open_gl->glGetUniformLocation(program, "ftex");
    
    result.light_color_loc = open_gl->glGetUniformLocation(program, "light_color");
    
    result.position_loc = 0;
    result.color_loc    = 1;
    result.uv_loc       = 2;
    result.normal_loc   = 3;
    result.loaded       = true;
    result.full_name    = copy_string(filepath);
    
    char *short_name = copy_string(filepath);
    int t = find_character_from_right(short_name, '/');
    while (t >= 0) {
        advance(&short_name, t + 1);
        t = find_character_from_right(short_name, '/');
    }
    Break_String_Result r = break_by_tok(short_name, '.');
    result.short_name     = short_name;
    return result;
}

internal void
unload_shader(Shader *shader) {
    if (!shader->loaded) return;
    
    open_gl->glDeleteProgram(shader->program);
    
    shader->loaded = false;
}

internal void
perform_reloads(Shader_Catalog *catalog) {
    int count = sb_count(catalog->base.short_names_to_reload);
    for (int i = 0; i < count; ++i) {
        char *name = catalog->base.short_names_to_reload[i];
        Shader *shader_to_reload = find_shader(name);
        if (shader_to_reload) {
            reload_shader(shader_to_reload);
        }
    }
    
    sb_free(catalog->base.short_names_to_reload);
    sb_free(catalog->base.full_names_to_reload);
    
    catalog->base.short_names_to_reload = 0;
    catalog->base.full_names_to_reload = 0;
}

internal void
reload_shader(Shader *shader) {
    OutputDebugString("Reloading shader '");
    OutputDebugString(shader->short_name); 
    OutputDebugString("'...");
    if (shader->program) {
        unload_shader(shader);
    }
    assert(shader->full_name);
    
    Shader reloaded_shader = load_shader(shader->full_name);
    platform_free(shader->full_name);
    platform_free(shader->short_name);
    
    *shader = reloaded_shader;
    OutputDebugString(" Reloaded.\n");
}

internal void
reload_shaders() {
    if (global_shader.program) 
        unload_shader(&global_shader);
    if (global_basic_3d_light_shader.program)
        unload_shader(&global_basic_3d_light_shader);
    if (global_basic_3d_shader.program)
        unload_shader(&global_basic_3d_shader);
    if (global_screen_shader.program)
        unload_shader(&global_screen_shader);
    
    global_shader = load_shader("./data/shaders/argb_texture.glsl");
    global_basic_3d_shader = load_shader("./data/shaders/basic_3d.glsl");
    global_basic_3d_light_shader = load_shader("./data/shaders/basic_3d_light.glsl");
    global_screen_shader = load_shader("./data/shaders/screen.glsl");
}

internal void
init_shaders() {
    init_shader_catalog();
    reload_shaders();
    
    // Add to catalog
    add_shader(&global_shader);
    add_shader(&global_basic_3d_shader);
    add_shader(&global_basic_3d_light_shader);
    add_shader(&global_screen_shader);
}
