/* date = December 14th 2020 10:15 pm */

#define MESH_FLIP_UVS 0x01

struct Texture_Map {
    u32 width;
    u32 height;
    // flags field?
    u32 id;
    
    char *name;
    char *full_path;
    b32 loaded;
};

struct Texture_Catalog {
    Texture_Map *data;
    u32 size;
};

// TODO(diego): Replace with hash map
#define TEXTURE_CATALOG_SIZE 32
global_variable Texture_Catalog texture_catalog = {};

//
// TODO(diego): Move to specific file?
//

#define TEXTURE_MAP_NAME_COUNT 3

#define TEXTURE_MAP_DIFFUSE  0
#define TEXTURE_MAP_SPECULAR 1
#define TEXTURE_MAP_NORMAL   2

struct Render_Material {
    Vector3 ambient_color;
    Vector3 diffuse_color;
    Vector3 specular_color;
    
    real32 shininess;
    
    char *name;
    
    char *texture_map_names[TEXTURE_MAP_NAME_COUNT];
};

struct Triangle_List_Info {
    s32 start_index;
    u32 num_indices;
    s32 material_index;
    
    Texture_Map *diffuse_map;
    Texture_Map *specular_map;
    Texture_Map *normal_map;
};

struct Triangle_Mesh {
    char *filepath;
    
    // Vertex data
    Vector3 *vertices;
    Vector3 *normals;
    Vector2 *uvs;
    
    // Index data
    u32 *indices;
    
    Vector4 *colors;
    
    Triangle_List_Info *triangle_list_info;
    Render_Material    *material_info;
    
    // Count stuff.
    u32 vertex_count; // Count for vertices, normals and uvs
    u32 index_count;
    u32 texture_count;
    
    u32 material_info_count;
    u32 triangle_list_count;
    
    // OpenGL stuff.
    u32 vao;
    u32 vbo;
    u32 ebo;
};

//
// Mesh
//
internal void init_texture_catalog();

internal Texture_Map * find_texture(char *map_name);
internal Texture_Map * load_texture_map(char *filepath);
internal void load_textures_for_mesh(Triangle_Mesh *mesh);

internal void make_vertex_buffers(Triangle_Mesh *mesh);

internal void init_mesh(Triangle_Mesh *mesh);
internal void draw_mesh(Triangle_Mesh *mesh);

internal Triangle_Mesh gen_mesh_cube(real32 widht, real32 height, real32 length);
internal Triangle_Mesh load_mesh(char *filepath, uint32 flags);

internal void free_mesh(Triangle_Mesh *mesh);