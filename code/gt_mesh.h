/* date = December 14th 2020 10:15 pm */

//
// TODO(diego): Move to specific file?
//

struct Render_Material {
    v3 ambient_color;
    v3 diffuse_color;
};

enum Texture_Type {
    TextureType_None,
    TextureType_Diffuse,
    TextureType_Specular,
    TextureType_Count,
};

struct Texture {
    u32 id;
    Texture_Type type;
};

struct Triangle_List_Info {
    s32 material_index;
    s32 start_index;
    u32 num_indices;
};

struct Triangle_Mesh {
    char *filepath;
    
    // Vertex data
    v3 *vertices;
    v3 *normals;
    v2 *uvs;
    
    // Index data
    u32 *indices;
    
    v4 *colors;
    
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


internal Triangle_Mesh load_mesh_from_obj(char *filepath);

internal void init_mesh(Triangle_Mesh *mesh);

internal void draw_mesh(Triangle_Mesh *mesh);
internal void free_mesh(Triangle_Mesh *mesh);