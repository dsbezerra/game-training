#include <gl/gl.h>

typedef char GLchar;

typedef void type_glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
typedef void type_glGetShaderiv(GLuint shader, GLenum pname, GLint *params);
typedef void type_glGetProgramiv(GLuint program, GLenum pname, GLint *params);
typedef void type_glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
typedef void type_glShaderSource(GLuint shader, GLsizei count, GLchar **string, GLint *length);
typedef GLuint type_glCreateShader(GLenum type);
typedef void type_glCompileShader(GLuint shader);
typedef void type_glDetachShader(GLuint program, GLuint shader);
typedef void type_glDeleteShader(GLuint shader);
typedef int type_glCreateProgram();
typedef void type_glAttachShader(GLuint program, GLuint shader);
typedef GLuint type_glLinkProgram(GLuint program);
typedef void type_glUseProgram(GLuint program);
typedef void type_glDeleteProgram(GLuint program);
typedef void type_glGenBuffers(GLsizei n, GLuint * buffers);
typedef void type_glGenFramebuffers(GLsizei n, GLuint * ids);
typedef void type_glDeleteBuffers(GLsizei n, GLuint * buffers);
typedef void type_glBindVertexArray(GLuint array);
typedef void type_glBindBuffer(GLenum target, GLuint buffer);
typedef void type_glBindFramebuffer(GLenum target, GLuint framebuffer);
typedef void type_glBufferData(GLenum target, GLsizei size, const void * data, GLenum usage);
typedef void type_glBufferSubData(GLenum target, GLsizei size, const void * data);
typedef void type_glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void * pointer);
typedef void type_glGenVertexArrays(GLsizei n, GLuint *arrays);
typedef void type_glDeleteVertexArrays(GLsizei n, GLuint *arrays);

typedef void type_glEnableVertexAttribArray(GLuint index);
typedef void type_glDisableVertexAttribArray(GLuint index);

typedef void type_glActiveTexture(GLenum texture);

typedef void type_glDrawArrays(GLenum mode, GLint first, GLsizei count);
typedef void type_glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices);

typedef GLenum type_glCheckFramebufferStatus(GLenum target);
typedef void type_glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void type_glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef void type_glTexImage2DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);

typedef void type_glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);


typedef void type_glDeleteFramebuffers(GLsizei n, GLuint *framebuffers);

typedef GLint type_glGetUniformLocation(GLuint program, const GLchar *name);
typedef void type_glUniform1i(GLint location, GLint v0);
typedef void type_glUniform1f(GLint location, GLfloat v0);
typedef void type_glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void type_glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void type_glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, GLfloat *value);

typedef void type_glGenerateMipmap(GLenum target);

typedef GLuint type_glGetDebugMessageLog(GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog);

typedef BOOL type_wglSwapIntervalEXT(int interval);

#define opengl_function(name) type_##name *name
#define opengl_get_function(name) open_gl->name = (type_##name *) wglGetProcAddress(#name)

struct Opengl_Info {
    char *vendor;
    char *renderer;
    char *version;
};

struct Opengl_Memory_Info {
    GLint total_memory_in_kb;
    GLint current_available_memory_in_kb;
};

struct Opengl {
    Opengl_Info info;
    
    GLint max_multi_sample_count;
    
    opengl_function(glGetShaderInfoLog);
    opengl_function(glGetShaderiv);
    opengl_function(glGetProgramiv);
    opengl_function(glGetProgramInfoLog);
    opengl_function(glShaderSource);
    opengl_function(glCreateShader);
    opengl_function(glCompileShader);
    opengl_function(glDetachShader);
    opengl_function(glDeleteShader);
    opengl_function(glCreateProgram);
    opengl_function(glAttachShader);
    opengl_function(glLinkProgram);
    opengl_function(glUseProgram);
    opengl_function(glDeleteProgram);
    opengl_function(glGenBuffers);
    
    opengl_function(glGenFramebuffers);
    
    opengl_function(glDeleteBuffers);
    opengl_function(glBindVertexArray);
    opengl_function(glBindBuffer);
    opengl_function(glBindFramebuffer);
    
    opengl_function(glDeleteFramebuffers);
    
    opengl_function(glBufferData);
    opengl_function(glBufferSubData);
    opengl_function(glVertexAttribPointer);
    opengl_function(glGenVertexArrays);
    opengl_function(glDeleteVertexArrays);
    opengl_function(glUniform1f);
    opengl_function(glUniform3f);
    opengl_function(glUniform4f);
    opengl_function(glUniformMatrix4fv);
    opengl_function(glDrawArrays);
    opengl_function(glDrawElements);
    opengl_function(glGetUniformLocation);
    opengl_function(glEnableVertexAttribArray);
    opengl_function(glDisableVertexAttribArray);
    opengl_function(glActiveTexture);
    opengl_function(glUniform1i);
    opengl_function(glGenerateMipmap);
    opengl_function(glGetDebugMessageLog);
    
    opengl_function(glCheckFramebufferStatus);
    opengl_function(glFramebufferTexture2D);
    opengl_function(glFramebufferRenderbuffer);
    
    opengl_function(glBlitFramebuffer);
    
    opengl_function(glTexImage2DMultisample);
    opengl_function(wglSwapIntervalEXT);
};

struct Opengl_Framebuffer {
    GLuint framebuffer_handle;
    GLuint color_handle;
    GLuint depth_handle;
};

enum Opengl_Framebuffer_Flags
{
    OpenGLFramebuffer_Multisampled = 0x1,
    OpenGLFramebuffer_Filtered = 0x2,
    OpenGLFramebuffer_Depth = 0x4,
    OpenGLFramebuffer_Float = 0x8,
};

internal Opengl_Info        opengl_get_info();
// internal Opengl_Memory_Info opengl_get_memory_info();

internal void opengl_init();

internal GLuint framebuffer_tex_image(Opengl *open_gl, GLuint slot, u32 width, u32 height, GLint filter_type, GLuint format);

internal Opengl_Framebuffer create_framebuffer(u32 width, u32 height, u32 flags, u32 color_buffer_count);