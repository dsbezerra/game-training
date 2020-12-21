global_variable Immediate* immediate;
global_variable Mat4 view_matrix;
global_variable Mat4 projection_matrix;

global_variable u32 draw_call_count = 0;

#define DUMP_GL_ERRORS true

internal void
init_draw() {
    init_shaders();
    immediate_init();
    set_shader(global_shader);
}

internal void
deinit_draw() {
    immediate_free();
}

internal void
immediate_init() {
    if (immediate) {
        return;
    }
    
    immediate = (Immediate *) platform_alloc(sizeof(Immediate));
    
#if DUMP_GL_ERRORS
#define GL_DEBUG_OUTPUT 0x92E0
    glEnable(GL_DEBUG_OUTPUT);
#endif
    
    open_gl->glGenVertexArrays(1, &immediate->vao);
    open_gl->glBindVertexArray(immediate->vao);
    
    open_gl->glGenBuffers(1, &immediate->vbo);
    open_gl->glBindBuffer(GL_ARRAY_BUFFER, immediate->vbo);
    
    open_gl->glBindVertexArray(0);
    open_gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    
    //glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

internal void
immediate_free() {
    platform_free(immediate);
}

internal void
set_Mat4(char *name, Mat4 matrix) {
    GLint location = open_gl->glGetUniformLocation(immediate->current_shader.program, name);
    open_gl->glUniformMatrix4fv(location, 1, GL_FALSE, matrix.e);
}

internal void
set_int1(char *name, int value) {
    GLint location = open_gl->glGetUniformLocation(immediate->current_shader.program, name);
    open_gl->glUniform1i(location, value);
}

internal void
set_float(char *name, real32 value) {
    GLint location = open_gl->glGetUniformLocation(immediate->current_shader.program, name);
    open_gl->glUniform1f(location, value);
}

internal void
set_float3(char *name, Vector3 values) {
    GLint location = open_gl->glGetUniformLocation(immediate->current_shader.program, name);
    open_gl->glUniform3f(location, values.x, values.y, values.z);
}

internal void
set_float4(char *name, Vector4 values) {
    GLint location = open_gl->glGetUniformLocation(immediate->current_shader.program, name);
    open_gl->glUniform4f(location, values.x, values.y, values.z, values.w);
}

internal void
immediate_begin() {
    assert(immediate);
    
    immediate->num_vertices = 0;
}

internal Vertex * 
get_next_vertex_ptr() {
	return (Vertex *) &immediate->vertices + immediate->num_vertices;
}

internal void
immediate_vertex(Vector3 position, Vector4 color, Vector2 uv) {
    immediate_vertex(position, color, uv, make_vector3(.0f, 0.f, .0f));
}

internal void
immediate_vertex(Vector3 position, Vector4 color, Vector2 uv, Vector3 normal) {
    
    if (immediate->num_vertices >= MAX_VERTICES) {
		immediate_flush();
		immediate_begin();
	}
    
    Vertex *v = get_next_vertex_ptr();
    v->position.x = position.x;
    if (immediate->mode == RenderingMode_2D) {
        v->position.y = -position.y;
    } else {
        v->position.y = position.y;
    }
    v->position.z = position.z;
    v->color      = color;
    v->uv         = uv;
    v->normal     = normal;
    
    immediate->num_vertices += 1;
}

internal void
immediate_circle(Vector2 center, real32 inner_radius_x, real32 inner_radius_y, real32 outer_radius_x, real32 outer_radius_y, Vector4 color) {
    Vector2 default_uv = make_vector2(-1.f, -1.f);
    
    int amount = (int) max_real32(outer_radius_x, outer_radius_y);
    for (int i = 0; i < amount; ++i) {
        int next = i + 1;
        
        // Inner coordinates
        real32 inner_ax = center.x + (inner_radius_x * cosf(i * TAU / amount));
        real32 inner_ay = center.y + (inner_radius_y * sinf(i * TAU / amount));
        real32 inner_bx = center.x + (inner_radius_x * cosf(next * TAU / amount));
        real32 inner_by = center.y + (inner_radius_y * sinf(next * TAU / amount));
        
        // Outer coordinates
        real32 outer_ax = center.x + (outer_radius_x * cosf(i * TAU / amount));
        real32 outer_ay = center.y + (outer_radius_y * sinf(i * TAU / amount));
        real32 outer_bx = center.x + (outer_radius_x * cosf(next * TAU / amount));
        real32 outer_by = center.y + (outer_radius_y * sinf(next * TAU / amount));
        
        // Triangle made with 2 vertices in inner radius to one vertex in outer radius 
        immediate_vertex(make_vector3(inner_ax, inner_ay, 1.f), color, default_uv);
        immediate_vertex(make_vector3(inner_bx, inner_by, 1.f), color, default_uv);
        immediate_vertex(make_vector3(outer_bx, outer_by, 1.f), color, default_uv);
        
        // Triangle made with 2 vertices in outer radius to one vertex in inner radius
        immediate_vertex(make_vector3(outer_ax, outer_ay, 1.f), color, default_uv);
        immediate_vertex(make_vector3(outer_bx, outer_by, 1.f), color, default_uv);
        immediate_vertex(make_vector3(inner_ax, inner_ay, 1.f), color, default_uv);
    }
}

internal void
immediate_circle(Vector2 center, real32 inner_radius, real32 outer_radius, Vector4 color) {
    immediate_circle(center, inner_radius, inner_radius, outer_radius, outer_radius, color);
}

internal void
immediate_circle(Vector2 center, Vector2 inner_radius, Vector2 outer_radius, Vector4 color) {
    immediate_circle(center, inner_radius.x, inner_radius.y, outer_radius.x, outer_radius.y, color);
}

internal void
immediate_circle_filled(Vector2 center, real32 radius_x, real32 radius_y, Vector4 color) {
    // NOTE(diego): To get a smooth and anti-aliased circle I guess we will 
    // need a custom shader.
    // https://blog.lapingames.com/draw-circle-glsl-shader/
    Vector2 default_uv = make_vector2(-1.f, -1.f);
    int amount = (int) (radius_x > radius_y ? radius_x : radius_y);
    for (int i = 0; i < amount; ++i) {
        real32 ax = center.x + (radius_x * cosf(i * TAU / amount));
        real32 ay = center.y + (radius_y * sinf(i * TAU / amount));
        
        int next = i + 1;
        real32 bx = center.x + (radius_x * cosf(next * TAU / amount));
        real32 by = center.y + (radius_y * sinf(next * TAU / amount));
        
        // Vertices to form a slice
        immediate_vertex(make_vector3(center.x, center.y, 1.f), color, default_uv);
        immediate_vertex(make_vector3(ax, ay, 1.f), color, default_uv);
        immediate_vertex(make_vector3(bx, by, 1.f), color, default_uv);
    }
}

internal void
immediate_circle_filled(Vector2 center, Vector2 radius, Vector4 color) {
    immediate_circle_filled(center, radius.x, radius.y, color);
}

internal void
immediate_circle_filled(Vector2 center, real32 radius, Vector4 color) {
    immediate_circle_filled(center, radius, radius, color);
}

internal void
immediate_quad(Vector2 min, Vector2 max, Vector4 color) {
    immediate_quad(min.x, min.y, max.x, max.y, color);
}

internal void
immediate_quad(real32 x0, real32 y0, real32 x1, real32 y1, Vector4 color) {
    Vector2 default_uv = make_vector2(-1.f, -1.f);
    real32 z_index = 1.f;
    immediate_vertex(make_vector3(x0, y0, z_index), color, default_uv);
    immediate_vertex(make_vector3(x0, y1, z_index), color, default_uv);
    immediate_vertex(make_vector3(x1, y0, z_index), color, default_uv);
    
    immediate_vertex(make_vector3(x0, y1, z_index), color, default_uv);
    immediate_vertex(make_vector3(x1, y1, z_index), color, default_uv);
    immediate_vertex(make_vector3(x1, y0, z_index), color, default_uv);
}


internal void
immediate_textured_quad(Vector2 min, Vector2 max, u32 texture) {
    
    glBindTexture(GL_TEXTURE_2D, texture);
    open_gl->glActiveTexture(GL_TEXTURE0);
    
    Vector2 top_right    = make_vector2(1.f, 1.f);
    Vector2 top_left     = make_vector2(.0f, 1.f);
    Vector2 bottom_right = make_vector2(1.f, .0f);
    Vector2 bottom_left  = make_vector2(.0f, .0f);
    
    real32 z_index = 1.f;
    Vector4 color = make_color(0xffffffff);
    
    immediate_vertex(make_vector3(min.x, max.y, z_index), color, bottom_left);
    immediate_vertex(make_vector3(max.x, max.y, z_index), color, bottom_right);
    immediate_vertex(make_vector3(min.x, min.y, z_index), color, top_left);
    
    immediate_vertex(make_vector3(min.x, min.y, z_index), color, top_left);
    immediate_vertex(make_vector3(max.x, min.y, z_index), color, top_right);
    immediate_vertex(make_vector3(max.x, max.y, z_index), color, bottom_right);
}

internal void
immediate_char(real32 x, real32 y, char c, Loaded_Font *font, Vector4 color) {
    
    glBindTexture(GL_TEXTURE_2D, font->texture);
    open_gl->glActiveTexture(GL_TEXTURE0);
    
    if (c >= 32 && c < 255) {
        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(font->cdata, BITMAP_SIZE, BITMAP_SIZE, c - 32, &x, &y, &q, 1);
        
        Vector2 bottom_right = make_vector2(q.s1, q.t0);
        Vector2 bottom_left  = make_vector2(q.s0, q.t0);
        Vector2 top_right    = make_vector2(q.s1, q.t1);
        Vector2 top_left     = make_vector2(q.s0, q.t1);
        
        real32 z_index  = 1.f;
        
        immediate_vertex(make_vector3(q.x0, q.y0, z_index), color, bottom_left);
        immediate_vertex(make_vector3(q.x0, q.y1, z_index), color, top_left);
        immediate_vertex(make_vector3(q.x1, q.y0, z_index), color, bottom_right);
        
        immediate_vertex(make_vector3(q.x0, q.y1, z_index), color, top_left);
        immediate_vertex(make_vector3(q.x1, q.y1, z_index), color, top_right);
        immediate_vertex(make_vector3(q.x1, q.y0, z_index), color, bottom_right);
    }
}

internal void
immediate_text(real32 x, real32 y, u8 *text, Loaded_Font *font, Vector4 color, real32 z_index) {
    
    glBindTexture(GL_TEXTURE_2D, font->texture);
    open_gl->glActiveTexture(GL_TEXTURE0);
    
    real32 start = x;
    
    while (*text) {
        if (*text >= 32 && *text < 255) {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(font->cdata, BITMAP_SIZE, BITMAP_SIZE, *text - 32, &x, &y, &q, 1);
            
            Vector2 bottom_right = make_vector2(q.s1, q.t0);
            Vector2 bottom_left  = make_vector2(q.s0, q.t0);
            Vector2 top_right    = make_vector2(q.s1, q.t1);
            Vector2 top_left     = make_vector2(q.s0, q.t1);
            
            immediate_vertex(make_vector3(q.x0, q.y0, z_index), color, bottom_left);
            immediate_vertex(make_vector3(q.x0, q.y1, z_index), color, top_left);
            immediate_vertex(make_vector3(q.x1, q.y0, z_index), color, bottom_right);
            
            immediate_vertex(make_vector3(q.x0, q.y1, z_index), color, top_left);
            immediate_vertex(make_vector3(q.x1, q.y1, z_index), color, top_right);
            immediate_vertex(make_vector3(q.x1, q.y0, z_index), color, bottom_right);
            
        } else if (*text == '\n') {
            y += font->line_height;
            x = start;
        }
        ++text;
    }
}

internal void
immediate_flush() {
    assert(immediate);
    
    int count = immediate->num_vertices;
    if (count == 0) {
        return;
    }
    
    if (!immediate->current_shader.program) {
        // TODO(diego): Log messages!
        immediate->num_vertices = 0;
        return;
    }
    
    draw_call_count++;
    
    open_gl->glBindVertexArray(immediate->vao);
    open_gl->glBindBuffer(GL_ARRAY_BUFFER, immediate->vbo);
    open_gl->glBufferData(GL_ARRAY_BUFFER, sizeof(immediate->vertices[0]) * count, immediate->vertices, GL_STREAM_DRAW);
    
    GLint position_loc = immediate->current_shader.position_loc;
    GLint color_loc = immediate->current_shader.color_loc;
    GLint uv_loc = immediate->current_shader.uv_loc;
    GLint normal_loc = immediate->current_shader.normal_loc;
    
    open_gl->glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    open_gl->glEnableVertexAttribArray(position_loc);
    
    open_gl->glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) sizeof(Vector3));
    open_gl->glEnableVertexAttribArray(color_loc);
    
    open_gl->glVertexAttribPointer(uv_loc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3) + sizeof(Vector4)));
    open_gl->glEnableVertexAttribArray(uv_loc);
    
    open_gl->glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3) + sizeof(Vector4) + sizeof(Vector2)));
    open_gl->glEnableVertexAttribArray(normal_loc);
    
    glDrawArrays(GL_TRIANGLES, 0, immediate->num_vertices);
    dump_gl_errors("immediate_flush");
    
    open_gl->glDisableVertexAttribArray(position_loc);
    open_gl->glDisableVertexAttribArray(color_loc);
    open_gl->glDisableVertexAttribArray(uv_loc);
    
    open_gl->glBindVertexArray(0);
    open_gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

internal void
draw_text(real32 x, real32 y, u8 *text, Loaded_Font *font, Vector4 color) {
    immediate_begin();
    immediate_text(x, y, text, font, color, -5.f);
    immediate_flush();
}

internal void
set_shader(Shader new_shader) {
    assert(immediate);
    
    immediate->current_shader = new_shader;
    open_gl->glUseProgram(new_shader.program);
}

internal void
refresh_shader_transform() {
    if (!immediate || !immediate->current_shader.program) {
        return;
    }
    
    set_Mat4("view", view_matrix);
    set_Mat4("projection", projection_matrix);
}

internal void
render_2d_right_handed(int width, int height) {
    immediate->mode = RenderingMode_2D;
    
    // NOTE(diego): This shader is reused for all 2D rendering.
    set_shader(global_shader);
    
#if 0
    Mat4 tm = identity();
    
    tm.rc[0][0] = 2.f / width;
    tm.rc[1][1] = 2.f / height;
    
    tm.rc[3][0] = -10.f;
    tm.rc[3][1] = 10.f;
    
    projection_matrix = tm;
    view_matrix       = identity();
#else 
    real32 aspect_ratio = (real32) width / (real32) height;
    
    real32 f = 10.f;
    real32 n = -10.f;
    
    real32 ortho_size = height / 2.f;
    
    projection_matrix = ortho(ortho_size, aspect_ratio, f, n);
    view_matrix       = translate(make_vector2(-width / 2.f, ortho_size));
#endif
    
    refresh_shader_transform();
}

internal void
render_3d(int width, int height, real32 fov = 45.f) {
    immediate->mode = RenderingMode_3D;
    
    real32 aspect_ratio = (real32) width / (real32) height;
    real32 n = 0.1f;
    real32 f = 100.f;
    
    projection_matrix = perspective(fov, aspect_ratio, f, n);
    view_matrix       = identity();
    
    refresh_shader_transform();
}

internal void
dump_gl_errors(char *tag) {
#if DUMP_GL_ERRORS
    if (!open_gl->glGetDebugMessageLog) return;
    
    while (true) {
        GLuint source, type, id;
        GLint length;
        GLenum severity;
        
        GLchar buffer[4096];
        
        if (open_gl->glGetDebugMessageLog(1, sizeof(buffer), &source, &type, &id, &severity, &length, buffer)) {
            
            char buf[4096];
            sprintf(buf, "[%s] %s\n", tag, buffer);
            OutputDebugString(buf);
        } else {
            break;
        }
    }
#endif
}