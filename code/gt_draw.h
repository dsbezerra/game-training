#define MAX_VERTICES (3 * 1024)

struct vertex {
    v2 position;
    v4 color;
    v2 uv;
    real32 z_index;
};

struct imm {
    shader current_shader;
    
    GLuint vao;
    GLuint vbo;
    
    vertex vertices[MAX_VERTICES];
    
    int num_vertices;
};

internal void init_draw();
internal void draw();
internal void deinit_draw();

internal void immediate_init();
internal void immediate_free();

internal void immediate_begin();
internal void immediate_flush();

internal vertex *get_next_vertex_ptr();

internal void immediate_vertex(v2 position, v4 color, v2 uv, real32 z_index);


internal void immediate_circle(v2 center, real32 inner_radius_x, real32 inner_radius_y, real32 outer_radius_x, real32 outer_radius_y, v4 color);
internal void immediate_circle(v2 center, real32 inner_radius, real32 outer_radius, v4 color);
internal void immediate_circle(v2 center, v2 inner_radius, v2 outer_radius, v4 color);


internal void immediate_circle_filled(v2 center, real32 radius_x, real32 radius_y, v4 color);
internal void immediate_circle_filled(v2 center, real32 radius, v4 color);
internal void immediate_circle_filled(v2 center, v2 radius, v4 color);

internal void immediate_quad(v2 min, v2 max, v4 color, real32 z_index);
internal void immediate_quad(real32 x0, real32 y0, real32 x1, real32 y1, v4 color, real32 z_index);
internal void immediate_text(real32 x, real32 y, u8 *text, loaded_font *font, v4 color);

internal void draw_text(real32 x, real32 y, u8 *text, loaded_font *font, v4 color);


internal void set_shader(shader new_shader);
internal void refresh_shader_transform();
internal void render_right_handed(int width, int height);