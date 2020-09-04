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
typedef void type_glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void type_glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, GLfloat *value);

typedef BOOL type_wglSwapIntervalEXT(int interval);

#define opengl_function(name) type_##name *name
#define opengl_get_function(name) open_gl->name = (type_##name *) wglGetProcAddress(#name)

struct opengl_info {
    char *vendor;
    char *renderer;
    char *version;
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
    opengl_function(glUniform4f);
    opengl_function(glUniformMatrix4fv);
    opengl_function(glDrawArrays);
    opengl_function(glGetUniformLocation);
    opengl_function(glEnableVertexAttribArray);
    opengl_function(glDisableVertexAttribArray);
    opengl_function(glActiveTexture);
    opengl_function(glUniform1i);
    opengl_function(wglSwapIntervalEXT);
};
