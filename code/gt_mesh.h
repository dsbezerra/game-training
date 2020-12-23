/* date = December 14th 2020 10:15 pm */

enum Texture_Type {
    TextureType_None,
    TextureType_Diffuse,
    TextureType_Specular,
    TextureType_Count,
};

struct Texture_Map {
    u32 id;
    Texture_Type type;
    b32 loaded;
};

//
// TODO(diego): Move to specific file?
//

struct Render_Material {
    Vector3 ambient_color;
    Vector3 diffuse_color;
    Vector3 specular_color;
    
    real32 shininess;
    
    char *name;
    
    Texture_Map *diffuse_map;
    Texture_Map *specular_map;
};

#define MAX_MATERIALS 1000
Render_Material materials[MAX_MATERIALS];

struct Triangle_List_Info {
    s32 start_index;
    u32 num_indices;
    s32 material_index;
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
    
    Triangle_List_Info *list;
    
    // Count stuff.
    u32 vertex_count; // Count for vertices, normals and uvs
    u32 index_count;
    u32 texture_count;
    
    u32 triangle_list_count;
    
    // OpenGL stuff.
    u32 vao;
    u32 vbo;
    u32 ebo;
};

//
// Materials
//
internal Render_Material * get_material_at(int index);
internal int find_material_index(char *name);
internal void clear_materials();

//
// Mesh
//
internal void init_mesh(Triangle_Mesh *mesh);
internal void draw_mesh(Triangle_Mesh *mesh);
internal Triangle_Mesh gen_mesh_cube(real32 widht, real32 height, real32 length);
internal Triangle_Mesh load_mesh(char *filepath);
internal void free_mesh(Triangle_Mesh *mesh);