#include <gl/gl.h>

// 
// GL Definitions
//

#define GL_FRAGMENT_SHADER      0x8B30
#define GL_VERTEX_SHADER        0x8B31
#define GL_COMPILE_STATUS       0x8B81
#define GL_LINK_STATUS          0x8B82
#define GL_INFO_LOG_LENGTH      0x8B84

// 
// Buffer Objects
//

#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893

#define GL_STREAM_DRAW                    0x88E0
#define GL_STATIC_DRAW                    0x88E4
#define GL_DYNAMIC_DRAW                   0x88E8

//
// Texture
//

#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE1                       0x84C1

//
// Other
//
#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX   0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

//
// Extensions
//
#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB             0x2093
#define WGL_CONTEXT_FLAGS_ARB                   0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126

#define WGL_CONTEXT_DEBUG_BIT_ARB               0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB  0x0002

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB        0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

#define WGL_DRAW_TO_WINDOW_ARB                  0x2001
#define WGL_ACCELERATION_ARB                    0x2003
#define WGL_SUPPORT_OPENGL_ARB                  0x2010
#define WGL_DOUBLE_BUFFER_ARB                   0x2011
#define WGL_PIXEL_TYPE_ARB                      0x2013

#define WGL_TYPE_RGBA_ARB                       0x202B
#define WGL_FULL_ACCELERATION_ARB               0x2027

#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB        0x20A9

#define WGL_RED_BITS_ARB                        0x2015
#define WGL_GREEN_BITS_ARB                      0x2017
#define WGL_BLUE_BITS_ARB                       0x2019
#define WGL_ALPHA_BITS_ARB                      0x201B
#define WGL_DEPTH_BITS_ARB                      0x2022

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
typedef void type_glGenBuffers(GLsizei n, GLuint * buffers);
typedef void type_glBindVertexArray(GLuint array);
typedef void type_glBindBuffer(GLenum target, GLuint buffer);
typedef void type_glBufferData(GLenum target, GLsizei size, const void * data, GLenum usage);
typedef void type_glBufferSubData(GLenum target, GLsizei size, const void * data);
typedef void type_glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void * pointer);
typedef void type_glGenVertexArrays(GLsizei n, GLuint *arrays);

typedef void type_glEnableVertexAttribArray(GLuint index);
typedef void type_glDisableVertexAttribArray(GLuint index);

typedef void type_glActiveTexture(GLenum texture);

typedef void type_glDrawArrays(GLenum mode, GLint first, GLsizei count);

typedef GLint type_glGetUniformLocation(GLuint program, const GLchar *name);
typedef void type_glUniform1i(GLint location, GLint v0);
typedef void type_glUniform1f(GLint location, GLfloat v0);
typedef void type_glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void type_glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void type_glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, GLfloat *value);

typedef void type_glGenerateMipmap(GLenum target);

typedef BOOL type_wglSwapIntervalEXT(int interval);


typedef HGLRC WINAPI wgl_create_context_attribs_arb(HDC hDC, HGLRC hShareContext,
                                                    const int *attribList);

typedef BOOL WINAPI wgl_get_pixel_format_attrib_iv_arb(HDC hdc,
                                                       int iPixelFormat,
                                                       int iLayerPlane,
                                                       UINT nAttributes,
                                                       const int *piAttributes,
                                                       int *piValues);

typedef BOOL WINAPI wgl_get_pixel_format_attrib_fv_arb(HDC hdc,
                                                       int iPixelFormat,
                                                       int iLayerPlane,
                                                       UINT nAttributes,
                                                       const int *piAttributes,
                                                       FLOAT *pfValues);

typedef BOOL WINAPI wgl_choose_pixel_format_arb(HDC hdc,
                                                const int *piAttribIList,
                                                const FLOAT *pfAttribFList,
                                                UINT nMaxFormats,
                                                int *piFormats,
                                                UINT *nNumFormats);

#define opengl_function(name) type_##name *name
#define opengl_get_function(name) open_gl->name = (type_##name *) wglGetProcAddress(#name)

struct opengl_info {
    char *vendor;
    char *renderer;
    char *version;
};

struct opengl_memory_info {
    GLint total_memory_in_kb;
    GLint current_available_memory_in_kb;
};

struct opengl {
    opengl_info info;
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
    opengl_function(glGenBuffers);
    opengl_function(glBindVertexArray);
    opengl_function(glBindBuffer);
    opengl_function(glBufferData);
    opengl_function(glBufferSubData);
    opengl_function(glVertexAttribPointer);
    opengl_function(glGenVertexArrays);
    opengl_function(glUniform1f);
    opengl_function(glUniform3f);
    opengl_function(glUniform4f);
    opengl_function(glUniformMatrix4fv);
    opengl_function(glDrawArrays);
    opengl_function(glGetUniformLocation);
    opengl_function(glEnableVertexAttribArray);
    opengl_function(glDisableVertexAttribArray);
    opengl_function(glActiveTexture);
    opengl_function(glUniform1i);
    opengl_function(glGenerateMipmap);
    opengl_function(wglSwapIntervalEXT);
};
