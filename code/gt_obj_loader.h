/* date = December 16th 2020 7:11 pm */

enum Obj_Element_Kind {
    ObjElementKind_None,
    ObjElementKind_Object,
    ObjElementKind_Group,
    ObjElementKind_Vertex,
    ObjElementKind_UV,
    ObjElementKind_Normal,
    ObjElementKind_Face,
    ObjElementKind_Usemtl,
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
        Vector3 vertex;
        Vector3 normal;
        Vector2 uv;
        
        Obj_Face_Spec face_spec;
        
        char *object_name;
        char *group_name;
        char *material_name;
    };
};

struct Obj_Material {
    Vector3 Ka;
    Vector3 Kd;
    Vector3 Ks;
    
    float Ns;
    
    char *name;
    
    char *texture_map_names[TEXTURE_MAP_NAME_COUNT];
};

struct Obj_Index_Entry {
    Obj_Index key;
    u32 value;
    Obj_Index_Entry *next;
};

struct Obj_Index_Table {
    Obj_Index_Entry **entries;
    u64 size;
};

struct Obj_Model {
    char *filepath;
    
    Vector3 *vertices;
    Vector3 *normals;
    Vector2 *uvs;
    
    Obj_Face_Spec *faces;
    Obj_Element *elements;
    Obj_Material *materials;
    Obj_Index_Table *table;
    
    // Count
    u32 vertex_count;
    u32 normal_count;
    u32 uv_count;
    u32 face_count;
    
    u32 object_count;
    u32 group_count;
    u32 material_count;
    u32 element_count;
    
    u32 material_index;
    
    s32 previous_usemtl_index;
    s32 usemtl_index;
    
    u32 inserting_count;
};

//
// Private
//
internal int find_material_index(Obj_Model *model, char *name);
internal void parse_mtl(Obj_Model *model);
internal Triangle_Mesh load_mesh_from_obj(char *filepath, uint32 flags);

internal u64 hash(Obj_Index_Table *table, Obj_Index idx);
internal Obj_Index_Table* index_table_create(u64 size);
internal Obj_Index_Entry* index_table_get(Obj_Index_Table *table, Obj_Index key);
internal void index_table_set(Obj_Index_Table *table, Obj_Index key, u32 value);
internal Obj_Index_Entry* index_table_pair(Obj_Index key, u32 value);
internal void index_table_free(Obj_Index_Table *table);

internal void release(Obj_Model *model);

inline b32
operator==(Obj_Index a, Obj_Index b) {
    b32 result = false;
    
    result = (a.vertex_index == b.vertex_index && 
              a.uv_index == b.uv_index && 
              a.normal_index == b.normal_index);
    
    return result;
}

inline b32
operator!=(Obj_Index a, Obj_Index b) {
    return !(a == b);
}

internal int get_index(int index, int count);
internal void fix_face_indices(Obj_Model *model, Obj_Face_Spec *spec);
internal void insert_face(Triangle_Mesh *mesh, Obj_Model *model, Obj_Face_Spec *face);
internal void insert_vertex(Triangle_Mesh *mesh, Obj_Model *model, Obj_Index idx);
