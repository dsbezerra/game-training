/* date = December 14th 2020 10:15 pm */

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

enum Obj_Element_Kind {
    ObjElementKind_None,
    ObjElementKind_Vertex,
    ObjElementKind_UV,
    ObjElementKind_Normal,
    ObjElementKind_Face,
    ObjElementKind_Count,
};

struct Obj_Index {
    u32 vertex_index;
    u32 normal_index;
    u32 uv_index;
};

struct Obj_Face_Spec {
    Obj_Index idx0;
    Obj_Index idx1;
    Obj_Index idx2;
};

struct Obj_Element {
    Obj_Element_Kind kind;
    union {
        v3 vertex;
        v3 normal;
        v2 uv;
        Obj_Face_Spec face_spec;
    };
};

struct Obj_Model {
    char *filepath;
    
    v3 *vertices;
    v3 *normals;
    v2 *uvs;
    Obj_Face_Spec *faces;
    
    u32 vertex_count;
    u32 normal_count;
    u32 uv_count;
    u32 face_count;
};

// TODO(diego): Move to gt_model.cpp?
struct Model {
    char *filepath;
    
    Triangle_Mesh *meshes;
    u64 mesh_count;
};

internal void init_model(Model *model);
internal void init_mesh(Triangle_Mesh *mesh);

internal void draw_mesh(Triangle_Mesh *mesh);

internal Model load_obj_model(char *filepath);

internal Model load_model(char *filepath);
internal void free_mesh(Triangle_Mesh *mesh);