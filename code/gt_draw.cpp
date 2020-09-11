global_variable imm* immediate;
global_variable mat4 view_matrix;
global_variable mat4 projection_matrix;

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
immediate_vertex(v2 position, v4 color, v2 uv, real32 z_index) {
    
    if (immediate->num_vertices >= MAX_VERTICES) {
		immediate_flush();
		immediate_begin();
	}
    
    vertex *v = get_next_vertex_ptr();
    v->position.x = position.x;
    v->position.y = -position.y;
    v->color      = color;
    v->uv         = uv;
    v->z_index    = z_index;
    
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
        immediate_vertex(make_v2(inner_ax, inner_ay), color, default_uv, 1.f);
        immediate_vertex(make_v2(inner_bx, inner_by), color, default_uv, 1.f);
        immediate_vertex(make_v2(outer_bx, outer_by), color, default_uv, 1.f);
        
        // Triangle made with 2 vertices in outer radius to one vertex in inner radius
        immediate_vertex(make_v2(outer_ax, outer_ay), color, default_uv, 1.f);
        immediate_vertex(make_v2(outer_bx, outer_by), color, default_uv, 1.f);
        immediate_vertex(make_v2(inner_ax, inner_ay), color, default_uv, 1.f);
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
        immediate_vertex(center, color, default_uv, 1.f);
        immediate_vertex(make_v2(ax, ay), color, default_uv, 1.f);
        immediate_vertex(make_v2(bx, by), color, default_uv, 1.f);
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
immediate_quad(v2 min, v2 max, v4 color, real32 z_index) {
    immediate_quad(min.x, min.y, max.x, max.y, color, z_index);
}

internal void
immediate_quad(real32 x0, real32 y0, real32 x1, real32 y1, v4 color, real32 z_index) {
    v2 default_uv = make_v2(-1.f, -1.f);
    immediate_vertex(make_v2(x0, y0), color, default_uv, z_index);
    immediate_vertex(make_v2(x0, y1), color, default_uv, z_index);
    immediate_vertex(make_v2(x1, y0), color, default_uv, z_index);
    
    immediate_vertex(make_v2(x0, y1), color, default_uv, z_index);
    immediate_vertex(make_v2(x1, y1), color, default_uv, z_index);
    immediate_vertex(make_v2(x1, y0), color, default_uv, z_index);
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
            
            immediate_vertex(make_v2(q.x0, q.y0), color, bottom_left, z_index);
            immediate_vertex(make_v2(q.x0, q.y1), color, top_left, z_index);
            immediate_vertex(make_v2(q.x1, q.y0), color, bottom_right, z_index);
            
            immediate_vertex(make_v2(q.x0, q.y1), color, top_left, z_index);
            immediate_vertex(make_v2(q.x1, q.y1), color, top_right, z_index);
            immediate_vertex(make_v2(q.x1, q.y0), color, bottom_right, z_index);
            
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
    
    open_gl->glBindVertexArray(immediate->vao);
    open_gl->glBindBuffer(GL_ARRAY_BUFFER, immediate->vbo);
    open_gl->glBufferData(GL_ARRAY_BUFFER, sizeof(immediate->vertices[0]) * count, immediate->vertices, GL_STREAM_DRAW);
    
    GLint position_loc = immediate->current_shader.position_loc;
    GLint color_loc = immediate->current_shader.color_loc;
    GLint uv_loc = immediate->current_shader.uv_loc;
    GLint z_index_loc = immediate->current_shader.z_index_loc;
    
    open_gl->glVertexAttribPointer(position_loc, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);
    open_gl->glEnableVertexAttribArray(position_loc);
    
    open_gl->glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) sizeof(v2));
    open_gl->glEnableVertexAttribArray(color_loc);
    
    open_gl->glVertexAttribPointer(uv_loc, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(sizeof(v2) + sizeof(v4)));
    open_gl->glEnableVertexAttribArray(uv_loc);
    
    open_gl->glVertexAttribPointer(z_index_loc, 1, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(sizeof(v2) + sizeof(v4) + sizeof(v2)));
    open_gl->glEnableVertexAttribArray(z_index_loc);
    
    glDrawArrays(GL_TRIANGLES, 0, immediate->num_vertices);
    
    open_gl->glDisableVertexAttribArray(position_loc);
    open_gl->glDisableVertexAttribArray(color_loc);
    open_gl->glDisableVertexAttribArray(uv_loc);
    open_gl->glDisableVertexAttribArray(z_index_loc);
    
    open_gl->glBindVertexArray(0);
    open_gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

internal void
draw_text(real32 x, real32 y, u8 *text, loaded_font *font, v4 color) {
    immediate_begin();
    immediate_text(x, y, text, font, color, 1.f);
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
render_right_handed(int width, int height) {
    mat4 tm = identity();
    tm.rc[0][0] = 2.f / width;
    tm.rc[1][1] = 2.f / height;
    
    tm.rc[3][0] = -1.f;
    tm.rc[3][1] = 1.f;
    
    projection_matrix = tm;
    view_matrix       = identity();
    
    refresh_shader_transform();
}
