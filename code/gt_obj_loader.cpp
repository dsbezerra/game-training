#include "gt_obj_loader.h"

#define MAX_MATERIALS 1000
Obj_Material materials[MAX_MATERIALS];

internal void
clear_materials() {
    for (int i = 0; i < array_count(materials); i++) {
        Obj_Material *material = &materials[i];
        if (material->name) platform_free(material->name);
        if (material->texture_map_name) platform_free(material->texture_map_name);
    }
}

internal Obj_Material*
get_material_at(int index) {
    if (index < 0 || index >= MAX_MATERIALS) {
        return 0;
    }
    Obj_Material *mat = &materials[index];
    if (!mat->name) return 0;
    
    return mat;
}

internal int
find_material_index(char *name) {
    // @Speed make this use hash table if necessary.
    for (int i = 0; i < MAX_MATERIALS; i++) {
        Obj_Material *mat = &materials[i];
        if (strings_are_equal(mat->name, name))
            return i;
    }
    return -1;
}

internal void
parse_mtl(char *filepath) {
    int len = string_length(filepath);
    char *mtlpath = (char *) platform_alloc(len * sizeof(char *));
    copy_string(mtlpath, filepath);
    put_string_at(mtlpath, "mtl", len - 3);
    
    File_Contents obj_file = platform_read_entire_file(mtlpath);
    assert(obj_file.file_size > 0);
    
    // We don't need mtlpath anymore.
    platform_free(mtlpath);
    
    
    Text_File_Handler handler = {};
    init_handler(&handler, obj_file.contents);
    
    assert(handler.num_lines > 0);
    
    Obj_Material current_material = {};
    
    u32 newmtl_count = 0;
    
    u8 *at = obj_file.contents;
    while (1) {
        
        char *line = consume_next_line(&handler);
        if (!line) break;
        
        Break_String_Result r = break_by_spaces(line);
        if (!r.lhs || r.lhs[0] == '#') {
            //
            // First we try to get the Material count from comment at the second line
            //
            // # Material Count: X
            //
            if (r.rhs[0] == 'M' && !materials) {
                r = break_by_tok(r.rhs, ':'); 
                if (strings_are_equal(r.lhs, "Material Count")) {
                    int count = atoi(r.rhs);
                    assert(count < MAX_MATERIALS);
                }
            }
            continue;
        } else if (strings_are_equal(r.lhs, "newmtl")) {
            if (newmtl_count > 0) {
                materials[newmtl_count - 1] = current_material;
            }
            //
            // We assume that all of our mtl files have # Material Count in the second comment.
            //
            if (materials) {
                current_material.name = (char *) platform_alloc(string_length(r.rhs) * sizeof(char *));
                copy_string(current_material.name, r.rhs);
            }
            newmtl_count++;
        } else if (strings_are_equal(r.lhs, "Ns")) {
            sscanf(r.rhs, "%f", &current_material.Ns);
        } else if (strings_are_equal(r.lhs, "Ka")) {
            sscanf(r.rhs, "%f %f %f", &current_material.Ka.r, &current_material.Ka.g, &current_material.Ka.b);
        } else if (strings_are_equal(r.lhs, "Kd")) {
            sscanf(r.rhs, "%f %f %f", &current_material.Kd.r, &current_material.Kd.g, &current_material.Kd.b);
        } else if (strings_are_equal(r.lhs, "Ks")) {
            sscanf(r.rhs, "%f %f %f", &current_material.Ks.r, &current_material.Ks.g, &current_material.Ks.b);
        } else if (strings_are_equal(r.lhs, "Ke")) {
            // Ignored.
        } else if (strings_are_equal(r.lhs, "Ni")) {
            // Ignored.
        } else if (strings_are_equal(r.lhs, "d")) {
            // Ignored.
        } else if (strings_are_equal(r.lhs, "illum")) {
            // Ignored.
        } else if (strings_are_equal(r.lhs, "map_Kd")) {
            // TODO(diego): Get only the texture name!!
            current_material.texture_map_name = (char *) platform_alloc(string_length(r.rhs) * sizeof(char *));
            copy_string(current_material.texture_map_name, r.rhs);
        }
    }
    
    // Add last one
    if (newmtl_count > 0) {
        materials[newmtl_count - 1] = current_material;
    }
    
    platform_free(handler.data);
}

internal void
free_elements(Obj_Element *elements, u32 count) {
    if (!elements) return;
    
    for (u32 index = 0; index < count; ++index) {
        Obj_Element *e = &elements[index];
        if (!e) continue;
        switch (e->kind) {
            case ObjElementKind_Object: if (e->object_name)   platform_free(e->object_name);    break;
            case ObjElementKind_Group:  if (e->group_name)    platform_free(e->group_name);     break;
            case ObjElementKind_Usemtl: if (e->material_name) platform_free(e->material_name);  break;
            default: {
                // Continue
            } break;
        }
    }
    
    platform_free(elements);
}

internal Triangle_Mesh
load_mesh_from_obj(char *filepath) {
    assert(string_length(filepath) != 0);
    assert(string_ends_with(filepath, ".obj"));
    
    File_Contents obj_file = platform_read_entire_file(filepath);
    assert(obj_file.file_size > 0);
    
    u32 num_lines = count_lines(obj_file.contents);
    assert(num_lines > 0);
    
    Obj_Model model = {};
    Obj_Element *elements = (Obj_Element *) platform_alloc(num_lines * sizeof(Obj_Element));
    
    u32 current_line = 0;
    u8 *at = obj_file.contents;
    while (1) {
        
        char *line = consume_next_line(&at);
        if (!line) break;
        
        Obj_Element *element = &elements[current_line];
        
        Break_String_Result r = break_by_spaces(line);
        if (!r.lhs || r.lhs[0] == '#') {
            continue;
        } else if (strings_are_equal(r.lhs, "mtllib")) {
            parse_mtl(filepath);
        } else if (r.lhs[0] == 'o') {
            element->kind = ObjElementKind_Object;
            element->object_name = (char *) platform_alloc(string_length(r.rhs) * sizeof(char*));
            copy_string(element->object_name, r.rhs);
            model.object_count++;
            
        } else if (r.lhs[0] == 'g') {
            element->kind = ObjElementKind_Group;
            element->group_name = (char *) platform_alloc(string_length(r.rhs) * sizeof(char*));
            copy_string(element->group_name, r.rhs);
            model.group_count++;
            
        } else if (r.lhs[0] == 'v') {
            if (r.lhs[1] == 't') {
                Vector2 uv = {};
                sscanf(r.rhs, "%f %f", &uv.x, &uv.y);
                element->kind = ObjElementKind_UV;
                element->uv = uv;
                model.uv_count++;
                
            } else if (r.lhs[1] == 'n') {
                Vector3 normal = {};
                sscanf(r.rhs, "%f %f %f", &normal.x, &normal.y, &normal.z);
                
                element->kind = ObjElementKind_Normal;
                element->normal = normal;
                model.normal_count++;
            } else {
                Vector3 v = {};
                sscanf(r.rhs, "%f %f %f", &v.x, &v.y, &v.z);
                element->kind = ObjElementKind_Vertex;
                element->vertex = v;
                model.vertex_count++;
            }
            
        } else if (strings_are_equal(r.lhs, "usemtl")) {
            element->kind = ObjElementKind_Usemtl;
            element->material_name = (char *) platform_alloc(string_length(r.rhs) * sizeof(char));
            copy_string(element->material_name, r.rhs);
            model.material_count++;
            
        } else if (r.lhs[0] == 'f') {
            // NOTE(diego): We assume faces have Vertex index, uv index and normal index V/T/N
            // And that the model is already triangulated.
            Obj_Face_Spec spec = {};
            sscanf(r.rhs, "%d/%d/%d %d/%d/%d %d/%d/%d",
                   &spec.idx0.vertex_index, &spec.idx0.uv_index, &spec.idx0.normal_index,
                   &spec.idx1.vertex_index, &spec.idx1.uv_index, &spec.idx1.normal_index,
                   &spec.idx2.vertex_index, &spec.idx2.uv_index, &spec.idx2.normal_index);
            fix_face_indices(&model, &spec);
            element->kind = ObjElementKind_Face;
            element->face_spec = spec;
            
            model.face_count++;
            
        } else {
            element->kind = ObjElementKind_None;
        }
        
        ++current_line;
    }
    
    // NOTE(diego): Replace with error logging maybe.
    assert(model.vertex_count > 0);
    assert(model.face_count > 0);
    
    // Don't need it anymore
    platform_free(obj_file.contents);
    
    //
    // Load vertices, uvs, normals and faces into model structure.
    //
    model.vertices = (Vector3 *) platform_alloc(model.vertex_count * sizeof(Vector3));
    model.uvs      = (Vector2 *) platform_alloc(model.uv_count * sizeof(Vector2));
    model.normals  = (Vector3 *) platform_alloc(model.normal_count * sizeof(Vector3));
    model.faces    = (Obj_Face_Spec *) platform_alloc(model.face_count * sizeof(Obj_Face_Spec));
    
    model.table    = index_table_create(model.face_count * 3);
    
    model.usemtl_index = -1;
    
    
    //
    // Allocate result mesh here.
    //
    // We support single mesh for OBJ.
    Triangle_Mesh mesh = {};
    mesh.filepath = filepath;
    
    mesh.vertex_count = model.face_count * 3;
    mesh.index_count  = mesh.vertex_count;
    
    mesh.vertices  = (Vector3 *)  platform_alloc(mesh.vertex_count * sizeof(Vector3));
    mesh.indices   = (u32 *) platform_alloc(mesh.index_count  * sizeof(u32));
    mesh.uvs       = (Vector2 *)  platform_alloc(mesh.vertex_count * sizeof(Vector2));
    mesh.normals   = (Vector3 *)  platform_alloc(mesh.vertex_count * sizeof(Vector3));
    
    //
    // Setup triangle list
    //
    mesh.triangle_list_count = model.material_count;
    mesh.list      = (Triangle_List_Info *) platform_alloc(mesh.triangle_list_count * sizeof(Triangle_List_Info));
    for (u32 li = 0; li < mesh.triangle_list_count; ++li) {
        Triangle_List_Info *info = &mesh.list[li];
        info->start_index = -1;
        info->material_index = -1;
        info->num_indices = 0;
    }
    
    //
    // Input obj files vertices, uvs, normals and face counts.
    //
    // NOTE(diego): We construct the final mesh at the same time we fill
    // the Obj_Model structure.
    u32 v_count  = 0;
    u32 vt_count = 0;
    u32 vn_count = 0;
    u32 f_count = 0;
    
    for (u32 element_index = 0; element_index < num_lines; ++element_index) {
        Obj_Element *element = &elements[element_index];
        switch (element->kind) {
            case ObjElementKind_Vertex: {
                model.vertices[v_count++] = element->vertex;
            } break;
            case ObjElementKind_UV: {
                model.uvs[vt_count++] = element->uv;
            } break;
            case ObjElementKind_Normal: {
                model.normals[vn_count++] = element->normal;
            } break;
            case ObjElementKind_Usemtl: {
                model.previous_usemtl_index = model.usemtl_index;
                model.usemtl_index++;
                model.material_index = find_material_index(element->material_name);
            } break;
            case ObjElementKind_Face: {
                model.faces[f_count++] = element->face_spec;
                // Insert face to mesh
                insert_face(&mesh, &model, &element->face_spec);
            } break;
            default: break;
        }
    }
    
    //
    // Free stuff.
    //
    index_table_free(model.table);
    
    free_elements(elements, num_lines);
    
    if (model.vertices) platform_free(model.vertices);
    if (model.uvs)      platform_free(model.uvs);
    if (model.normals)  platform_free(model.normals);
    if (model.faces)    platform_free(model.faces);
    
    // Update last triangle list
    int previous_index = model.previous_usemtl_index == -1 ? 0 : model.usemtl_index;
    Triangle_List_Info *last = &mesh.list[previous_index];
    last->num_indices = model.inserting_count - last->start_index;
    
    return mesh;
}

//
// Obj_Index stuff
//

internal u64
hash(Obj_Index_Table *table, Obj_Index idx) {
    u64 value = 0;
    
    value = value * 37 + idx.vertex_index;
    value = value * 37 + idx.uv_index;
    value = value * 37 + idx.normal_index;
    
    return value % table->size;
}

internal Obj_Index_Table*
index_table_create(u64 size) {
    Obj_Index_Table *result = (Obj_Index_Table *) platform_alloc(sizeof(Obj_Index_Table));
    
    result->entries = (Obj_Index_Entry **) platform_alloc(sizeof(Obj_Index_Entry*) * size);
    result->size    = size;
    
    return result;
}

internal Obj_Index_Entry *
index_table_get(Obj_Index_Table *table, Obj_Index key) {
    u64 slot = hash(table, key);
    
    Obj_Index_Entry *entry = table->entries[slot];
    
    if (!entry) {
        return 0;
    }
    
    while (entry) {
        if (entry->key == key) {
            return entry;
        }
        entry = entry->next;
    }
    
    return 0;
}

internal Obj_Index_Entry *
index_table_pair(Obj_Index key, u32 value) {
    Obj_Index_Entry *entry = (Obj_Index_Entry *) platform_alloc(sizeof(Obj_Index_Entry));
    entry->key   = key;
    entry->value = value;
    entry->next  = 0;
    return entry;
}

internal void
index_table_set(Obj_Index_Table *table, Obj_Index key, u32 value) {
    u64 slot = hash(table, key);
    
    Obj_Index_Entry *entry = table->entries[slot];
    
    if (!entry) {
        table->entries[slot] = index_table_pair(key, value);
        return;
    }
    
    Obj_Index_Entry *prev = 0;
    
    while (entry) {
        if (entry->key == key) {
            entry->value = value;
            return;
        }
        prev  = entry;
        entry = prev->next;
    }
    
    if (prev) prev->next = index_table_pair(key, value);
}

internal void
index_table_free(Obj_Index_Table *table) {
    platform_free(table->entries);
    platform_free(table);
}

//
// Obj_Material stuff
//

internal u64
hash_djb2(u8 *str) {
    u64 hash = 5381;
    int c;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + (u64) (c);
    }
    
    return hash;
}

internal void
insert_vertex(Triangle_Mesh *mesh, Obj_Model *model, Obj_Index idx) {
    
    Vector3 vertex = model->vertices[idx.vertex_index];
    
    bool32 has_normals = model->normal_count > 0;
    bool32 has_uvs     = model->uv_count > 0;
    
    u32 count = model->inserting_count;
    
    Obj_Index_Entry *entry = index_table_get(model->table, idx);
    int previous = entry ? entry->value : -1;
    
    mesh->vertices[count] = vertex;
    if (model->uv_count > 0) {
        mesh->uvs[count] = model->uvs[idx.uv_index];
    }
    if (model->normal_count > 0) {
        mesh->normals[count] = model->normals[idx.normal_index];
    }
    
    if (previous != -1) {
        mesh->indices[count] = previous;
    } else {
        index_table_set(model->table, idx, count);
        mesh->indices[count] = count;
    }
    
    model->inserting_count++;
}

internal void
insert_face(Triangle_Mesh *mesh, Obj_Model *model, Obj_Face_Spec *face) {
    
    // Build triangle list
    if (model->usemtl_index > -1) {
        Triangle_List_Info *info = &mesh->list[model->usemtl_index];
        if (info->start_index == -1) {
            info->start_index = model->inserting_count;
            info->material_index = model->material_index;
        }
        if (model->previous_usemtl_index > -1) {
            if (model->usemtl_index != model->previous_usemtl_index) {
                Triangle_List_Info *prev = &mesh->list[model->previous_usemtl_index];
                prev->num_indices = model->inserting_count - prev->start_index;
            }
        }
    }
    
    insert_vertex(mesh, model, face->idx0);
    insert_vertex(mesh, model, face->idx1);
    insert_vertex(mesh, model, face->idx2);
}

internal int
get_index(int index, int count) {
    if (index > 0) {
        return index - 1;
    }
    return count + index;
}

internal void
fix_face_indices(Obj_Model *model, Obj_Face_Spec *spec) {
    assert(spec); assert(model);
    
    spec->idx0.vertex_index = get_index(spec->idx0.vertex_index, model->vertex_count);
    spec->idx1.vertex_index = get_index(spec->idx1.vertex_index, model->vertex_count);
    spec->idx2.vertex_index = get_index(spec->idx2.vertex_index, model->vertex_count);
    
    spec->idx0.uv_index = get_index(spec->idx0.uv_index, model->uv_count);
    spec->idx1.uv_index = get_index(spec->idx1.uv_index, model->uv_count);
    spec->idx2.uv_index = get_index(spec->idx2.uv_index, model->uv_count);
    
    spec->idx0.normal_index = get_index(spec->idx0.normal_index, model->normal_count);
    spec->idx1.normal_index = get_index(spec->idx1.normal_index, model->normal_count);
    spec->idx2.normal_index = get_index(spec->idx2.normal_index, model->normal_count);
}
