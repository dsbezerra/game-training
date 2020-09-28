opengl *open_gl;

opengl_info  
opengl_get_info() {
    opengl_info result = {};
    result.vendor = (char *) glGetString(GL_VENDOR);
    result.renderer = (char *) glGetString(GL_RENDERER);
    result.version = (char *) glGetString(GL_VERSION);
    return result;
}

opengl_memory_info
opengl_get_memory_info() {
    opengl_memory_info result = {};
    glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &result.total_memory_in_kb);
    glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &result.current_available_memory_in_kb);
    return result;
}
