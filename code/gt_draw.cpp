global_variable imm* immediate;
global_variable mat4 view_matrix;
global_variable mat4 projection_matrix;

global_variable u32 draw_call_count = 0;

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
    
    immediate = (imm *) platform_alloc(sizeof(imm));
    
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
immediate_begin() {
    assert(immediate);
    
    immediate->num_vertices = 0;
}

internal vertex * 
get_next_vertex_ptr() {
	return (vertex *) &immediate->vertices + immediate->num_vertices;
}

internal void
immediate_vertex(v3 position, v4 color, v2 uv) {
    
    if (immediate->num_vertices >= MAX_VERTICES) {
		immediate_flush();
		immediate_begin();
	}
    
    vertex *v = get_next_vertex_ptr();
    v->position.x = position.x;
    v->position.y = -position.y;
    v->position.z = position.z;
    v->color      = color;
    v->uv         = uv;
    
    immediate->num_vertices += 1;
}

internal void
immediate_circle(v2 center, real32 inner_radius_x, real32 inner_radius_y, real32 outer_radius_x, real32 outer_radius_y, v4 color) {
    v2 default_uv = make_v2(-1.f, -1.f);
    
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
        immediate_vertex(make_v3(inner_ax, inner_ay, 1.f), color, default_uv);
        immediate_vertex(make_v3(inner_bx, inner_by, 1.f), color, default_uv);
        immediate_vertex(make_v3(outer_bx, outer_by, 1.f), color, default_uv);
        
        // Triangle made with 2 vertices in outer radius to one vertex in inner radius
        immediate_vertex(make_v3(outer_ax, outer_ay, 1.f), color, default_uv);
        immediate_vertex(make_v3(outer_bx, outer_by, 1.f), color, default_uv);
        immediate_vertex(make_v3(inner_ax, inner_ay, 1.f), color, default_uv);
    }
}

internal void
immediate_circle(v2 center, real32 inner_radius, real32 outer_radius, v4 color) {
    immediate_circle(center, inner_radius, inner_radius, outer_radius, outer_radius, color);
}

internal void
immediate_circle(v2 center, v2 inner_radius, v2 outer_radius, v4 color) {
    immediate_circle(center, inner_radius.x, inner_radius.y, outer_radius.x, outer_radius.y, color);
}

internal void
immediate_circle_filled(v2 center, real32 radius_x, real32 radius_y, v4 color) {
    // NOTE(diego): To get a smooth and anti-aliased circle I guess we will 
    // need a custom shader.
    // https://blog.lapingames.com/draw-circle-glsl-shader/
    v2 default_uv = make_v2(-1.f, -1.f);
    int amount = (int) (radius_x > radius_y ? radius_x : radius_y);
    for (int i = 0; i < amount; ++i) {
        real32 ax = center.x + (radius_x * cosf(i * TAU / amount));
        real32 ay = center.y + (radius_y * sinf(i * TAU / amount));
        
        int next = i + 1;
        real32 bx = center.x + (radius_x * cosf(next * TAU / amount));
        real32 by = center.y + (radius_y * sinf(next * TAU / amount));
        
        // Vertices to form a slice
        immediate_vertex(make_v3(center.x, center.y, 1.f), color, default_uv);
        immediate_vertex(make_v3(ax, ay, 1.f), color, default_uv);
        immediate_vertex(make_v3(bx, by, 1.f), color, default_uv);
    }
}

internal void
immediate_circle_filled(v2 center, v2 radius, v4 color) {
    immediate_circle_filled(center, radius.x, radius.y, color);
}

internal void
immediate_circle_filled(v2 center, real32 radius, v4 color) {
    immediate_circle_filled(center, radius, radius, color);
}

internal void
immediate_quad(v2 min, v2 max, v4 color) {
    immediate_quad(min.x, min.y, max.x, max.y, color);
}

internal void
immediate_quad(real32 x0, real32 y0, real32 x1, real32 y1, v4 color) {
    v2 default_uv = make_v2(-1.f, -1.f);
    real32 z_index = 1.f;
    immediate_vertex(make_v3(x0, y0, z_index), color, default_uv);
    immediate_vertex(make_v3(x0, y1, z_index), color, default_uv);
    immediate_vertex(make_v3(x1, y0, z_index), color, default_uv);
    
    immediate_vertex(make_v3(x0, y1, z_index), color, default_uv);
    immediate_vertex(make_v3(x1, y1, z_index), color, default_uv);
    immediate_vertex(make_v3(x1, y0, z_index), color, default_uv);
}


internal void
immediate_textured_quad(v2 min, v2 max, u32 texture) {
    
    open_gl->glUniform1i(immediate->current_shader.texture_loc, 0);
    
    glBindTexture(GL_TEXTURE_2D, texture);
    open_gl->glActiveTexture(GL_TEXTURE0);
    
    v2 top_right    = make_v2(1.f, 1.f);
    v2 top_left     = make_v2(.0f, 1.f);
    v2 bottom_right = make_v2(1.f, .0f);
    v2 bottom_left  = make_v2(.0f, .0f);
    
    real32 z_index = 1.f;
    v4 color = make_color(0xffffffff);
    
    immediate_vertex(make_v3(min.x, max.y, z_index), color, bottom_left);
    immediate_vertex(make_v3(max.x, max.y, z_index), color, bottom_right);
    immediate_vertex(make_v3(min.x, min.y, z_index), color, top_left);
    
    immediate_vertex(make_v3(min.x, min.y, z_index), color, top_left);
    immediate_vertex(make_v3(max.x, min.y, z_index), color, top_right);
    immediate_vertex(make_v3(max.x, max.y, z_index), color, bottom_right);
}

internal void
immediate_char(real32 x, real32 y, char c, loaded_font *font, v4 color) {
    
    open_gl->glUniform1i(immediate->current_shader.texture_loc, 0);
    
    glBindTexture(GL_TEXTURE_2D, font->texture);
    open_gl->glActiveTexture(GL_TEXTURE0);
    
    if (c >= 32 && c < 255) {
        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(font->cdata, BITMAP_SIZE, BITMAP_SIZE, c - 32, &x, &y, &q, 1);
        
        v2 bottom_right = make_v2(q.s1, q.t0);
        v2 bottom_left  = make_v2(q.s0, q.t0);
        v2 top_right    = make_v2(q.s1, q.t1);
        v2 top_left     = make_v2(q.s0, q.t1);
        
        real32 z_index  = 1.f;
        
        immediate_vertex(make_v3(q.x0, q.y0, z_index), color, bottom_left);
        immediate_vertex(make_v3(q.x0, q.y1, z_index), color, top_left);
        immediate_vertex(make_v3(q.x1, q.y0, z_index), color, bottom_right);
        
        immediate_vertex(make_v3(q.x0, q.y1, z_index), color, top_left);
        immediate_vertex(make_v3(q.x1, q.y1, z_index), color, top_right);
        immediate_vertex(make_v3(q.x1, q.y0, z_index), color, bottom_right);
    }
}

internal void
immediate_text(real32 x, real32 y, u8 *text, loaded_font *font, v4 color, real32 z_index) {
    
    open_gl->glUniform1i(immediate->current_shader.texture_loc, 0);
    
    glBindTexture(GL_TEXTURE_2D, font->texture);
    open_gl->glActiveTexture(GL_TEXTURE0);
    
    real32 start = x;
    
    while (*text) {
        if (*text >= 32 && *text < 255) {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(font->cdata, BITMAP_SIZE, BITMAP_SIZE, *text - 32, &x, &y, &q, 1);
            
            v2 bottom_right = make_v2(q.s1, q.t0);
            v2 bottom_left  = make_v2(q.s0, q.t0);
            v2 top_right    = make_v2(q.s1, q.t1);
            v2 top_left     = make_v2(q.s0, q.t1);
            
            immediate_vertex(make_v3(q.x0, q.y0, z_index), color, bottom_left);
            immediate_vertex(make_v3(q.x0, q.y1, z_index), color, top_left);
            immediate_vertex(make_v3(q.x1, q.y0, z_index), color, bottom_right);
            
            immediate_vertex(make_v3(q.x0, q.y1, z_index), color, top_left);
            immediate_vertex(make_v3(q.x1, q.y1, z_index), color, top_right);
            immediate_vertex(make_v3(q.x1, q.y0, z_index), color, bottom_right);
            
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
    
    open_gl->glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);
    open_gl->glEnableVertexAttribArray(position_loc);
    
    open_gl->glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) sizeof(v3));
    open_gl->glEnableVertexAttribArray(color_loc);
    
    open_gl->glVertexAttribPointer(uv_loc, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(sizeof(v3) + sizeof(v4)));
    open_gl->glEnableVertexAttribArray(uv_loc);
    
    glDrawArrays(GL_TRIANGLES, 0, immediate->num_vertices);
    
    open_gl->glDisableVertexAttribArray(position_loc);
    open_gl->glDisableVertexAttribArray(color_loc);
    open_gl->glDisableVertexAttribArray(uv_loc);
    
    open_gl->glBindVertexArray(0);
    open_gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

internal void
draw_text(real32 x, real32 y, u8 *text, loaded_font *font, v4 color) {
    immediate_begin();
    immediate_text(x, y, text, font, color, -5.f);
    immediate_flush();
}

internal void
set_shader(shader new_shader) {
    assert(immediate);
    
    immediate->current_shader = new_shader;
    open_gl->glUseProgram(new_shader.program);
}

internal void
refresh_shader_transform() {
    if (!immediate || !immediate->current_shader.program) {
        return;
    }
    
    open_gl->glUniformMatrix4fv(immediate->current_shader.view_loc, 1, GL_FALSE, view_matrix.e);
    open_gl->glUniformMatrix4fv(immediate->current_shader.projection_loc, 1, GL_FALSE, projection_matrix.e);
}

internal void
render_2d_right_handed(int width, int height) {
    
    // NOTE(diego): This shader is reused for all 2D rendering.
    set_shader(global_shader);
    
#if 0
    mat4 tm = identity();
    
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
    view_matrix       = translate(make_v2(-width / 2.f, ortho_size));
#endif
    
    refresh_shader_transform();
}

internal void
render_3d(int width, int height, real32 fov = 45.f) {
    
    set_shader(global_basic_3d_shader);
    
    real32 aspect_ratio = (real32) width / (real32) height;
    real32 n = .1f;
    real32 f = 100.f;
    
    projection_matrix = perspective(fov, aspect_ratio, f, n);
    view_matrix       = identity();
    
    refresh_shader_transform();
}
