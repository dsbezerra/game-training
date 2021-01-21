#define SHADOW_WIDTH 2048 // If we crank this up to 5000 is already starts to look good.
#define SHADOW_HEIGHT 2048

#define MAX_VERTICES (3 * 1024)

struct Vertex {
    Vector3 position;
    Vector4 color;
    Vector2 uv;
    Vector3 normal;
};

enum Rendering_Mode {
    RenderingMode_None,
    RenderingMode_2D,
    RenderingMode_3D,
    RenderingMode_Count,
};

struct Immediate {
    Shader current_shader;
    
    Texture_Map fbo_map;
    Texture_Map menu_fbo_map;
    Texture_Map screen_fbo_map;
    
    Texture_Map depth_map;
    
    GLuint vao;
    GLuint vbo;
    
    Vertex vertices[MAX_VERTICES];
    
    int num_vertices;
    
    Rendering_Mode mode;
    
    Opengl_Framebuffer menu_framebuffer;
    Opengl_Framebuffer multisampled_framebuffer;
    Opengl_Framebuffer screen_framebuffer;
    Opengl_Framebuffer depth_framebuffer;
    
    Opengl_Framebuffer *framebuffer_to_blit;
};

internal void init_depth_map();

internal void init_draw();
internal void draw();
internal void deinit_draw();

internal void immediate_init();
internal void immediate_free();

internal void immediate_begin();
internal void immediate_flush();

internal Vertex *get_next_vertex_ptr();

internal void immediate_vertex(Vector3 position, Vector4 color, Vector2 uv);
internal void immediate_vertex(Vector3 position, Vector4 color, Vector2 uv, Vector3 normal);


internal void immediate_circle(Vector2 center, real32 inner_radius_x, real32 inner_radius_y, real32 outer_radius_x, real32 outer_radius_y, Vector4 color);
internal void immediate_circle(Vector2 center, real32 inner_radius, real32 outer_radius, Vector4 color);
internal void immediate_circle(Vector2 center, Vector2 inner_radius, Vector2 outer_radius, Vector4 color);


internal void immediate_circle_filled(Vector3 center, real32 radius_x, real32 radius_y, Vector4 color);
internal void immediate_circle_filled(Vector3 center, real32 radius, Vector4 color);
internal void immediate_circle_filled(Vector3 center, Vector2 radius, Vector4 color);

internal void immediate_textured_quad(Vector2 min, Vector2 max, u32 texture);


internal void immediate_quad(Vector2 min, Vector2 max, real32 z, Vector4 color);
internal void immediate_quad(Vector2 min, Vector2 max, Vector4 color);
internal void immediate_quad(real32 x0, real32 y0, real32 x1, real32 y1, Vector4 color);


internal void immediate_char(real32 x, real32 y, char c, Loaded_Font *font, Vector4 color);
internal void immediate_text(real32 x, real32 y, u8 *text, Loaded_Font *font, Vector4 color);

internal void immediate_cube(Vector3 position, u32 texture);

internal void draw_text(real32 x, real32 y, u8 *text, Loaded_Font *font, Vector4 color);


internal void set_shader(Shader new_shader);
internal void refresh_shader_transform();


internal void set_mat4(char *name, Mat4 matrix);

internal void set_int1(char *name, int value);
internal void set_float(char *name, real32 value);
internal void set_float3(char *name, Vector3 values);
internal void set_float4(char *name, Vector4 values);

internal void render_2d_right_handed(int width, int height);
internal void render_3d(int width, int height, real32 fov);

internal void dump_gl_errors(char *tag);

internal void use_framebuffer(GLuint id);
internal void use_framebuffer(Opengl_Framebuffer *framebuffer);

internal void refresh_framebuffer(int width, int height);
