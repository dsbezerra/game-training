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

internal Model
load_model(char *filepath) {
    Model result = {};
    
#if 0
    // TODO(diego): Replace this with own code?
    tinyobj_attrib_t attrib = {0};
    tinyobj_shape_t *meshes = 0;
    u32 mesh_count = 0;
    
    tinyobj_material_t *materials = 0;
    u32 material_count = 0;
    
    file_contents file = plataform_read_entire_file(filepath);
    
    u32 flags = TINYOBJ_FLAG_TRIANGULATE;
    int ret = tinyobj_parse_obj(&attrib, &meshes, &mesh_count, &materials, &material_count, (const char*) file.contents, file.file_size, flags);
    assert(ret == TINYOBJ_SUCCESS);
    
    // TODO(diego): Init model materials array
    
    if (material_count > 0) {
        result.mesh_count = material_count;
    } else {
        result.mesh_count = 1;
    }
    // material goes here
    
    result.meshes = (Triangle_Mesh *) platform_alloc(result.mesh_count * sizeof(Triangle_Mesh));
    
    // count the faces for each material
    int *faces = (int *) platform_alloc(result.mesh_count * sizeof(int));
    
    for (unsigned int mi = 0; mi < result.mesh_count; mi++) {
        for (unsigned int fi = 0; fi < meshes[mi].length; fi++) {
            int idx = attrib.material_ids[meshes[mi].face_offset + fi];
            if (idx == -1) idx = 0; // for no material face (which could be the whole model)
            faces[idx]++;
        }
    }
    
    
    int *v_count  = (int*) platform_alloc(result.mesh_count * sizeof(int));
    int *vt_count = (int*) platform_alloc(result.mesh_count * sizeof(int));
    int *vn_count = (int*) platform_alloc(result.mesh_count *sizeof(int));
    
    // allocate space for each of the material meshes
    for (int mi = 0; mi < result.mesh_count; mi++) {
        result.meshes[mi].vertex_count = faces[mi] * 3;
        result.meshes[mi].index_count = faces[mi] * 3;
        result.meshes[mi].vertices = (v3 *) platform_alloc(result.meshes[mi].vertex_count * sizeof(v3));
        result.meshes[mi].uvs = (v2 *) platform_alloc(result.meshes[mi].vertex_count * sizeof(v2));
        result.meshes[mi].normals = (v3 *) platform_alloc(result.meshes[mi].vertex_count * sizeof(v3));
        result.meshes[mi].indices = (u32 *) platform_alloc(result.meshes[mi].index_count * sizeof(u32));
    }
    
    // scan through the combined sub meshes and pick out each material mesh
    for (unsigned int af = 0; af < attrib.num_faces; af++) {
        int mm = attrib.material_ids[af];   // mesh material for this face
        if (mm == -1) { mm = 0; }           // no material object..
        
        // Get indices for the face
        tinyobj_vertex_index_t idx0 = attrib.faces[3 * af + 0];
        tinyobj_vertex_index_t idx1 = attrib.faces[3 * af + 1];
        tinyobj_vertex_index_t idx2 = attrib.faces[3 * af + 2];
        
        // Fill vertices buffer
        real32 vx = attrib.vertices[idx0.v_idx*3 + 0];
        real32 vy = attrib.vertices[idx0.v_idx*3 + 1];
        real32 vz = attrib.vertices[idx0.v_idx*3 + 2];
        result.meshes[mm].vertices[v_count[mm]] = make_v3(vx, vy, vz);
        result.meshes[mm].indices[v_count[mm]] = idx0.v_idx;
        v_count[mm] += 1;
        
        vx = attrib.vertices[idx1.v_idx*3 + 0];
        vy = attrib.vertices[idx1.v_idx*3 + 1];
        vz = attrib.vertices[idx1.v_idx*3 + 2];
        result.meshes[mm].vertices[v_count[mm]] = make_v3(vx, vy, vz);
        result.meshes[mm].indices[v_count[mm]] = idx1.v_idx;
        v_count[mm] += 1;
        
        vx = attrib.vertices[idx2.v_idx*3 + 0];
        vy = attrib.vertices[idx2.v_idx*3 + 1];
        vz = attrib.vertices[idx2.v_idx*3 + 2];
        result.meshes[mm].vertices[v_count[mm]] = make_v3(vx, vy, vz);
        result.meshes[mm].indices[v_count[mm]] = idx2.v_idx;
        
        v_count[mm] += 1;
        
        if (attrib.num_texcoords > 0) {
            real32 ux = attrib.texcoords[idx0.vt_idx*2 + 0];
            real32 uy = attrib.texcoords[idx0.vt_idx*2 + 1];
            result.meshes[mm].uvs[vt_count[mm]] = make_v2(ux, uy);
            vt_count[mm] += 1;
            
            ux = attrib.texcoords[idx1.vt_idx*2 + 0];
            uy = attrib.texcoords[idx1.vt_idx*2 + 1];
            result.meshes[mm].uvs[vt_count[mm]] = make_v2(ux, uy);
            vt_count[mm] += 1;
            
            ux = attrib.texcoords[idx2.vt_idx*2 + 0];
            uy = attrib.texcoords[idx2.vt_idx*2 + 1];
            result.meshes[mm].uvs[vt_count[mm]] = make_v2(ux, uy);
            vt_count[mm] += 1;
        }
        
        if (attrib.num_normals > 0) {
            // Fill normals buffer (float) using vertex index of the face
            real32 nx = attrib.normals[idx0.vn_idx*3 + 0];
            real32 ny = attrib.normals[idx0.vn_idx*3 + 1];
            real32 nz = attrib.normals[idx0.vn_idx*3 + 2];
            result.meshes[mm].normals[vn_count[mm]] = make_v3(nx, ny, nz);
            vn_count[mm] += 1;
            
            nx = attrib.normals[idx1.vn_idx*3 + 0];
            ny = attrib.normals[idx1.vn_idx*3 + 1];
            nz = attrib.normals[idx1.vn_idx*3 + 2];
            result.meshes[mm].normals[vn_count[mm]] = make_v3(nx, ny, nz);
            vn_count[mm] += 1;
            
            nx = attrib.normals[idx2.vn_idx*3 + 0];
            ny = attrib.normals[idx2.vn_idx*3 + 1];
            nz = attrib.normals[idx2.vn_idx*3 + 2];
            result.meshes[mm].normals[vn_count[mm]] = make_v3(nx, ny, nz);
            vn_count[mm] += 1;
        }
    }
    
    tinyobj_attrib_free(&attrib);
    tinyobj_shapes_free(meshes, mesh_count);
    tinyobj_materials_free(materials, material_count);
    
    platform_free(v_count);
    platform_free(vt_count);
    platform_free(vn_count);
    platform_free(faces);
    
    platform_free(file.contents);
    
#endif
    
    //
    // Init it.
    //
    result.mesh_count = 1;
    result.meshes = (Triangle_Mesh *) platform_alloc(sizeof(Triangle_Mesh));
    result.meshes[0] = gen_mesh_cube(1.f, 1.f, 1.f);
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