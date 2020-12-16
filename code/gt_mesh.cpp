internal void
init_model(Model *model) {
    assert(model);
    
    for (int mi = 0; mi < model->mesh_count; ++mi) {
        Triangle_Mesh *mesh = &model->meshes[mi];
        init_mesh(mesh);
    }
}

internal void
init_mesh(Triangle_Mesh *mesh) {
    assert(mesh);
    
    vertex *dest_buffer = new vertex[mesh->vertex_count];
    
    for (u32 index = 0; index < mesh->vertex_count; ++index) {
        vertex *dest = &dest_buffer[index];
        
        if (mesh->vertices) {
            dest->position = mesh->vertices[index];
        } else {
            dest->position = make_v3(0.f, 0.f, 0.f);
        }
        
        if (mesh->colors) {
            dest->color = mesh->colors[index];
        } else {
            dest->color = make_color(0xffff00ff);
        }
        
        if (mesh->uvs) {
            dest->uv = mesh->uvs[index];
        } else {
            dest->uv = make_v2(0.f, 0.f);
        }
        
        if (mesh->normals) {
            dest->normal = mesh->normals[index];
        } else {
            dest->normal = make_v3(0.f, 0.f, 0.f);
        }
    }
    
    // OpenGL stuff.
    open_gl->glGenVertexArrays(1, &mesh->vao);
    open_gl->glGenBuffers(1, &mesh->vbo);
    open_gl->glGenBuffers(1, &mesh->ebo);
    
    open_gl->glBindVertexArray(mesh->vao);
    open_gl->glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    
    open_gl->glBufferData(GL_ARRAY_BUFFER, sizeof(dest_buffer[0]) * mesh->vertex_count, dest_buffer, GL_STREAM_DRAW);  
    
    open_gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
    open_gl->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mesh->indices[0]) * mesh->index_count, 
                          mesh->indices, GL_STREAM_DRAW);
    
    GLint position_loc = immediate->current_shader.position_loc;
    GLint color_loc = immediate->current_shader.color_loc;
    GLint uv_loc = immediate->current_shader.uv_loc;
    GLint normal_loc = immediate->current_shader.normal_loc;
    
    open_gl->glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);
    open_gl->glEnableVertexAttribArray(position_loc);
    
    open_gl->glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) sizeof(v3));
    open_gl->glEnableVertexAttribArray(color_loc);	
    
    open_gl->glVertexAttribPointer(uv_loc, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(sizeof(v3) + sizeof(v4)));
    open_gl->glEnableVertexAttribArray(uv_loc);
    
    open_gl->glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(sizeof(v3) + sizeof(v4) + sizeof(v2)));
    open_gl->glEnableVertexAttribArray(normal_loc);	
    
    open_gl->glBindVertexArray(0);
    
    free(dest_buffer);
}

internal void
draw_model(Model *model) {
    
    for (int mi = 0; mi < model->mesh_count; ++mi) {
        Triangle_Mesh *mesh = &model->meshes[mi];
        draw_mesh(mesh);
    }
}

internal void
draw_mesh(Triangle_Mesh *mesh) {
    assert(mesh);
    
    open_gl->glBindVertexArray(mesh->vao);
    // TODO(diego): Add to open_gl
    open_gl->glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, 0);
    open_gl->glBindVertexArray(0);
}

internal Triangle_Mesh
gen_mesh_cube(float width, float height, float length) {
    Triangle_Mesh mesh = { 0 };
    
    v3 vertices[] = {
        -width/2, -height/2, length/2,
        width/2, -height/2, length/2,
        width/2, height/2, length/2,
        -width/2, height/2, length/2,
        -width/2, -height/2, -length/2,
        -width/2, height/2, -length/2,
        width/2, height/2, -length/2,
        width/2, -height/2, -length/2,
        -width/2, height/2, -length/2,
        -width/2, height/2, length/2,
        width/2, height/2, length/2,
        width/2, height/2, -length/2,
        -width/2, -height/2, -length/2,
        width/2, -height/2, -length/2,
        width/2, -height/2, length/2,
        -width/2, -height/2, length/2,
        width/2, -height/2, -length/2,
        width/2, height/2, -length/2,
        width/2, height/2, length/2,
        width/2, -height/2, length/2,
        -width/2, -height/2, -length/2,
        -width/2, -height/2, length/2,
        -width/2, height/2, length/2,
        -width/2, height/2, -length/2
    };
    
    float texcoords[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };
    
    float normals[] = {
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f,-1.0f,
        0.0f, 0.0f,-1.0f,
        0.0f, 0.0f,-1.0f,
        0.0f, 0.0f,-1.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f,-1.0f, 0.0f,
        0.0f,-1.0f, 0.0f,
        0.0f,-1.0f, 0.0f,
        0.0f,-1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f
    };
    
    mesh.vertices = (v3 *)platform_alloc(24*sizeof(v3));
    memcpy(mesh.vertices, vertices, 24*sizeof(v3));
    
    mesh.uvs = (v2 *)platform_alloc(24*sizeof(v2));
    memcpy(mesh.uvs, texcoords, 24*sizeof(v2));
    
    mesh.normals = (v3 *)platform_alloc(24*sizeof(v3));
    memcpy(mesh.normals, normals, 24*sizeof(v3));
    
    mesh.indices = (u32 *)platform_alloc(36*sizeof(u32));
    
    int k = 0;
    
    // Indices can be initialized right now
    for (int i = 0; i < 36; i+=6)
    {
        mesh.indices[i] = 4*k;
        mesh.indices[i+1] = 4*k+1;
        mesh.indices[i+2] = 4*k+2;
        mesh.indices[i+3] = 4*k;
        mesh.indices[i+4] = 4*k+2;
        mesh.indices[i+5] = 4*k+3;
        
        k++;
    }
    
    mesh.vertex_count = 24;
    mesh.index_count = 12 * 3;
    
    return mesh;
}

internal void
insert_vertex(Triangle_Mesh *mesh, Obj_Model *model, Obj_Index idx, u32 *vcount, u32 *ucount, u32 *ncount) {
    
    mesh->vertices[*vcount] = model->vertices[idx.vertex_index];
    mesh->indices[*vcount] = *vcount; // TODO(diego): Optimize it by getting the correct indices
    
    if (model->uv_count > 0) {
        mesh->uvs[(*ucount)++] = model->uvs[idx.uv_index];
    }
    if (model->normal_count > 0) {
        mesh->normals[(*ncount)++] = model->normals[idx.normal_index];
    }
    
    (*vcount)++;
}

internal int
get_index(int index, int count) {
    if (index > 0) {
        return index - 1;
    }
    return count + index;
}

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
            
            spec.idx0.vertex_index = get_index(spec.idx0.vertex_index, model.vertex_count);
            spec.idx1.vertex_index = get_index(spec.idx1.vertex_index, model.vertex_count);
            spec.idx2.vertex_index = get_index(spec.idx2.vertex_index, model.vertex_count);
            
            spec.idx0.uv_index = get_index(spec.idx0.uv_index, model.uv_count);
            spec.idx1.uv_index = get_index(spec.idx1.uv_index, model.uv_count);
            spec.idx2.uv_index = get_index(spec.idx2.uv_index, model.uv_count);
            
            spec.idx0.normal_index = get_index(spec.idx0.normal_index, model.normal_count);
            spec.idx1.normal_index = get_index(spec.idx0.normal_index, model.normal_count);
            spec.idx2.normal_index = get_index(spec.idx0.normal_index, model.normal_count);
            
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
    
    model.vertices = (v3 *) platform_alloc(model.vertex_count * sizeof(v3));
    model.uvs      = (v2 *) platform_alloc(model.uv_count * sizeof(v2));
    model.normals  = (v3 *) platform_alloc(model.normal_count * sizeof(v3));
    model.faces    = (Obj_Face_Spec *) platform_alloc(model.face_count * sizeof(Obj_Face_Spec));
    
    u32 v_count  = 0;
    u32 vt_count = 0;
    u32 vn_count = 0;
    u32 f_count = 0;
    
    Model result = {};
    result.mesh_count = 1;
    result.meshes = (Triangle_Mesh *) platform_alloc(result.mesh_count * sizeof(Triangle_Mesh));
    
    u32 vcount = 0;
    u32 ucount = 0;
    u32 ncount = 0;
    
    Triangle_Mesh *mesh = &result.meshes[0];
    
    u32 vertex_count = model.face_count * 3;
    u32 index_count = vertex_count * 3;
    mesh->vertices  = (v3 *) platform_alloc(vertex_count * sizeof(v3));
    mesh->indices   = (u32 *) platform_alloc(index_count * sizeof(u32));
    mesh->uvs       = (v2 *) platform_alloc(vertex_count * sizeof(v2));
    mesh->normals   = (v3 *) platform_alloc(vertex_count * sizeof(v3));
    
    mesh->vertex_count = vertex_count;
    mesh->index_count  = index_count;
    
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
                // Construct final model
                Obj_Index idx0 = element->face_spec.idx0;
                Obj_Index idx1 = element->face_spec.idx1;
                Obj_Index idx2 = element->face_spec.idx2;
                
                insert_vertex(mesh, &model, idx0, &vcount, &ucount, &ncount);
                insert_vertex(mesh, &model, idx1, &vcount, &ucount, &ncount);
                insert_vertex(mesh, &model, idx2, &vcount, &ucount, &ncount);
                
            } break;
            default: break;
        }
    }
    
    platform_free(elements);
    platform_free(model.vertices);
    platform_free(model.uvs);
    platform_free(model.normals);
    platform_free(model.faces);
    
    return result;
}

internal Model
load_model(char *filepath) {
    Model result = {};
    
#if 0
    result.mesh_count = 1;
    result.meshes = (Triangle_Mesh *) platform_alloc(sizeof(Triangle_Mesh));
    result.meshes[0] = gen_mesh_cube(1.f, 1.f, 1.f);
#else
    result = load_obj_model(filepath);
#endif
    result.filepath = filepath;
    
    //
    // Init it.
    //
    init_model(&result);
    
    return result;
}

internal void
free_mesh(Triangle_Mesh *mesh) {
    if (!mesh) return;
    
    if (mesh->vertices)    platform_free(mesh->vertices);
    if (mesh->normals)     platform_free(mesh->normals);
    if (mesh->uvs)         platform_free(mesh->uvs);
    if (mesh->indices)     platform_free(mesh->indices);
    
    // NOTE(diego): Maybe we don't want to delete textures because other objects
    // may use the same texture id.
    //
    // TODO(diego): Make sure that the texture is not being used by other meshes.
    if (mesh->textures) {
        // @Speed call glDeleteTextures with an array of textures.
        for (Texture *texture = mesh->textures; texture != mesh->textures + mesh->texture_count; texture++) {
            if (!texture) continue;
            glDeleteTextures(1, &texture->id);
        }
        platform_free(mesh->textures);
    }
    
    // Delete OpenGL stuff.
    open_gl->glDeleteVertexArrays(1, &mesh->vao);
    
    u32 gl_buffers[2] = {mesh->vbo, mesh->ebo};
    open_gl->glDeleteBuffers(array_count(gl_buffers), gl_buffers);
}