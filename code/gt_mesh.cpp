#include "gt_obj_loader.cpp"

internal void
init_mesh(Triangle_Mesh *mesh) {
    assert(mesh);
    
    Vertex *dest_buffer = new Vertex[mesh->vertex_count];
    
    for (u32 index = 0; index < mesh->vertex_count; ++index) {
        Vertex *dest = &dest_buffer[index];
        
        if (mesh->vertices) {
            dest->position = mesh->vertices[index];
        } else {
            dest->position = make_vector3(0.f, 0.f, 0.f);
        }
        
        if (mesh->colors) {
            dest->color = mesh->colors[index];
        } else {
            dest->color = make_color(0xffff00ff);
        }
        
        if (mesh->uvs) {
            dest->uv = mesh->uvs[index];
        } else {
            dest->uv = make_vector2(0.f, 0.f);
        }
        
        if (mesh->normals) {
            dest->normal = mesh->normals[index];
        } else {
            dest->normal = make_vector3(0.f, 0.f, 0.f);
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
    
    open_gl->glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    open_gl->glEnableVertexAttribArray(position_loc);
    
    open_gl->glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) sizeof(Vector3));
    open_gl->glEnableVertexAttribArray(color_loc);	
    
    open_gl->glVertexAttribPointer(uv_loc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3) + sizeof(Vector4)));
    open_gl->glEnableVertexAttribArray(uv_loc);
    
    open_gl->glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3) + sizeof(Vector4) + sizeof(Vector2)));
    open_gl->glEnableVertexAttribArray(normal_loc);	
    
    open_gl->glBindVertexArray(0);
    
    free(dest_buffer);
}

internal void
draw_mesh(Triangle_Mesh *mesh) {
    assert(mesh);
    
    open_gl->glBindVertexArray(mesh->vao);
    
    open_gl->glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    open_gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
    
    u32 index_size = sizeof(mesh->indices[0]);
    for (u32 li = 0; li < mesh->triangle_list_count; ++li) {
        Triangle_List_Info *tli = &mesh->triangle_list_info[li];
        if (tli->material_index > -1) {
            Render_Material *rm = &mesh->material_info[tli->material_index];
            set_float3("material.ambient", rm->ambient_color);
            set_float3("material.diffuse", rm->diffuse_color);
            set_float3("material.specular", rm->specular_color);
            set_float("material.shininess", rm->shininess);
        }
        
        s32 index = tli->start_index * index_size;
        open_gl->glDrawElements(GL_TRIANGLES, tli->num_indices, GL_UNSIGNED_INT, (void *) index);
    }
    open_gl->glBindVertexArray(0);
}

internal Triangle_Mesh
gen_mesh_cube(real32 width, real32 height, real32 length) {
    Triangle_Mesh mesh = { 0 };
    
    Vector3 vertices[] = {
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
    
    real32 texcoords[] = {
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
    
    real32 normals[] = {
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
    
    mesh.vertices = (Vector3 *)platform_alloc(24*sizeof(Vector3));
    memcpy(mesh.vertices, vertices, 24*sizeof(Vector3));
    
    mesh.uvs = (Vector2 *)platform_alloc(24*sizeof(Vector2));
    memcpy(mesh.uvs, texcoords, 24*sizeof(Vector2));
    
    mesh.normals = (Vector3 *)platform_alloc(24*sizeof(Vector3));
    memcpy(mesh.normals, normals, 24*sizeof(Vector3));
    
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

internal Triangle_Mesh
load_mesh(char *filepath) {
    Triangle_Mesh result = {};
    
#if 0
    result = gen_mesh_cube(1.f, 1.f, 1.f);
#else
    result = load_mesh_from_obj(filepath);
#endif
    result.filepath = filepath;
    
    //
    // Init it.
    //
    init_mesh(&result);
    
    return result;
}

internal void
free_mesh(Triangle_Mesh *mesh) {
    if (!mesh) return;
    
    if (mesh->vertices)    platform_free(mesh->vertices);
    if (mesh->normals)     platform_free(mesh->normals);
    if (mesh->uvs)         platform_free(mesh->uvs);
    if (mesh->indices)     platform_free(mesh->indices);
    
    if (mesh->triangle_list_info) {
        for (Triangle_List_Info *info = mesh->triangle_list_info; info != mesh->triangle_list_info + mesh->triangle_list_count; info++) {
            if (!info) continue;
            platform_free(info);
        }
        platform_free(mesh->triangle_list_info);
    }
    
    if (mesh->material_info) {
        for (Render_Material *rm = mesh->material_info; rm != mesh->material_info + mesh->material_info_count; rm++) {
            if (!rm) continue;
            if (rm->name) platform_free(rm->name);
            for (int n = 0; n < TEXTURE_MAP_NAME_COUNT; ++n) {
                if (rm->texture_map_names[n]) platform_free(rm->texture_map_names[n]);
            }
            platform_free(rm);
        }
        platform_free(mesh->triangle_list_info);
    }
    // Delete OpenGL stuff.
    open_gl->glDeleteVertexArrays(1, &mesh->vao);
    
    u32 gl_buffers[2] = {mesh->vbo, mesh->ebo};
    open_gl->glDeleteBuffers(array_count(gl_buffers), gl_buffers);
}