#include "gt_obj_loader.h"

internal Model
load_obj_model(char *filepath) {
    assert(string_length(filepath) != 0);
    assert(string_ends_with(filepath, ".obj"));
    
    file_contents obj_file = platform_read_entire_file(filepath);
    assert(obj_file.file_size > 0);
    
    Obj_Model model = {};
    model.filepath = filepath;
    
    u32 num_lines = count_lines(obj_file.contents);
    
    Obj_Element *elements = (Obj_Element *) platform_alloc(num_lines * sizeof(Obj_Element));
    
    u32 current_line = 0;
    
    u8 *at = obj_file.contents;
    while (1) {
        
        char *line = consume_next_line(&at);
        if (!line) break;
        
        Obj_Element *element = &elements[current_line];
        
        Break_String_Result r = break_by_spaces(line);
        if (strings_are_equal(r.lhs, "v")) {
            v3 vertex = {};
            sscanf(r.rhs, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
            
            element->kind = ObjElementKind_Vertex;
            element->vertex = vertex;
            model.vertex_count++;
        } else if (strings_are_equal(r.lhs, "vt")) {
            v2 uv = {};
            sscanf(r.rhs, "%f %f", &uv.x, &uv.y);
            element->kind = ObjElementKind_UV;
            element->uv = uv;
            model.uv_count++;
        } else if (strings_are_equal(r.lhs, "vn")) {
            v3 normal = {};
            sscanf(r.rhs, "%f %f %f", &normal.x, &normal.y, &normal.z);
            
            element->kind = ObjElementKind_Normal;
            element->normal = normal;
            model.normal_count++;
        } else if (strings_are_equal(r.lhs, "f")) {
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
    
    // Don't need it anymore
    platform_free(obj_file.contents);
    
    //
    // Load vertices, uvs, normals and faces into model structure.
    //
    model.vertices = (v3 *) platform_alloc(model.vertex_count * sizeof(v3));
    model.uvs      = (v2 *) platform_alloc(model.uv_count * sizeof(v2));
    model.normals  = (v3 *) platform_alloc(model.normal_count * sizeof(v3));
    model.faces    = (Obj_Face_Spec *) platform_alloc(model.face_count * sizeof(Obj_Face_Spec));
    
    model.table = index_table_create(model.face_count * 3);
    
    //
    // Allocate result mesh here.
    //
    //
    // We support single mesh for OBJ.
    Model result = {};
    result.mesh_count = 1;
    result.meshes = (Triangle_Mesh *) platform_alloc(result.mesh_count * sizeof(Triangle_Mesh));
    
    Triangle_Mesh *mesh = &result.meshes[0];
    
    mesh->vertex_count = model.face_count * 3;
    mesh->index_count  = mesh->vertex_count;
    
    mesh->vertices  = (v3 *)  platform_alloc(mesh->vertex_count * sizeof(v3));
    mesh->indices   = (u32 *) platform_alloc(mesh->index_count  * sizeof(u32));
    mesh->uvs       = (v2 *)  platform_alloc(mesh->vertex_count * sizeof(v2));
    mesh->normals   = (v3 *)  platform_alloc(mesh->vertex_count * sizeof(v3));
    
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
            case ObjElementKind_Face: {
                model.faces[f_count++] = element->face_spec;
                // Insert face to mesh
                insert_face(mesh, &model, &element->face_spec);
            } break;
            default: break;
        }
    }
    
    index_table_free(model.table);
    
    platform_free(elements);
    platform_free(model.vertices);
    platform_free(model.uvs);
    platform_free(model.normals);
    platform_free(model.faces);
    
    return result;
}

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

internal void
insert_vertex(Triangle_Mesh *mesh, Obj_Model *model, Obj_Index idx) {
    
    v3 vertex = model->vertices[idx.vertex_index];
    
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
