Opengl *open_gl;

Opengl_Info  
opengl_get_info() {
    Opengl_Info result = {};
    result.vendor = (char *) glGetString(GL_VENDOR);
    result.renderer = (char *) glGetString(GL_RENDERER);
    result.version = (char *) glGetString(GL_VERSION);
    return result;
}

Opengl_Memory_Info
opengl_get_memory_info() {
    Opengl_Memory_Info result = {};
    glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &result.total_memory_in_kb);
    glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &result.current_available_memory_in_kb);
    return result;
}
