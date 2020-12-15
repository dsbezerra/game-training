/* date = December 14th 2020 10:15 pm */

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "external/tinyobj_loader.h"      // OBJ/MTL file formats loading

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

struct Triangle_Mesh {
    // Vertex data
    v3 *vertices;
    v3 *normals;
    v2 *uvs;
    
    // Index data
    u32 *indices;
    
    v4 *colors;
    
    // Texture
    Texture *textures;
    
    // Count stuff.
    u32 vertex_count; // Count for vertices, normals and uvs
    u32 index_count;
    u32 texture_count;
    
    // OpenGL stuff.
    u32 vao;
    u32 vbo;
    u32 ebo;
};

// TODO(diego): Move to gt_model.cpp?
struct Model {
    Triangle_Mesh *meshes;
    u64 mesh_count;
};

internal void init_model(Model *model);
internal void init_mesh(Triangle_Mesh *mesh);

internal void draw_mesh(Triangle_Mesh *mesh);

internal Model load_model(char *filepath);
internal void free_mesh(Triangle_Mesh *mesh);