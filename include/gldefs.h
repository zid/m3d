#ifndef GLDEFS_H
#define GLDEFS_H
#include <stddef.h>
#include <gl/gl.h>

typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;
typedef char GLchar;

#define GL_DYNAMIC_STORAGE_BIT  0x0100
#define GL_CLAMP_TO_EDGE        0x812F
#define GL_R32UI                0x8236
#define GL_STATIC_DRAW          0x88E4
#define GL_ARRAY_BUFFER         0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_FRAGMENT_SHADER      0x8B30
#define GL_VERTEX_SHADER        0x8B31
#define GL_TEXTURE0             0x84C0
#define GL_TEXTURE1             0x84C1
#define GL_INFO_LOG_LENGTH      0x8B84
#define GL_TEXTURE_BUFFER       0x8C2A

/* Define this if you define these symbols */
#ifdef _GL_NO_EXTERN
#define ABI __attribute__((stdcall))
#else
#define ABI __attribute__((stdcall))
#endif

ABI void   (*glBindBuffer)(GLenum, GLuint);
ABI void   (*glGenBuffers)(GLsizei, GLuint *);
ABI void   (*glBufferData)(GLenum, GLsizeiptr, const GLvoid *, GLenum);
ABI void   (*glGenVertexArrays)(GLsizei, GLuint *);
ABI void   (*glBindVertexArray)(GLuint);
ABI void   (*glVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid *);
ABI void   (*glEnableVertexAttribArray)(GLuint);
ABI GLuint (*glCreateShader)(GLenum);
ABI void   (*glShaderSource)(GLuint, GLsizei, const GLchar **, const GLint *);
ABI void   (*glCompileShader)(GLuint);
ABI GLuint (*glCreateProgram)(void);
ABI void   (*glUseProgram)(GLuint);
ABI void   (*glLinkProgram)(GLuint);
ABI void   (*glAttachShader)(GLuint, GLuint);
ABI void   (*wglSwapIntervalEXT)(GLuint);
ABI void   (*glUniform1ui)(GLint, GLuint);
ABI void   (*glUniform1i)(GLint, GLuint);
ABI GLint  (*glGetUniformLocation)(GLuint, const GLchar *);
ABI void   (*glDeleteShader)(GLuint);
ABI void   (*glDeleteProgram)(GLuint);
ABI void   (*glActiveTexture)(GLenum);
ABI void   (*glGenerateMipmap)(GLuint);
ABI void   (*glGetShaderInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *);
ABI void   (*glGetShaderiv)(GLuint, GLenum, GLint *);
ABI void   (*glUniform1uiv)(GLint, GLsizei, const GLuint *);
ABI void   (*glUniform1iv)(GLint, GLsizei, const GLuint *);
ABI void   (*glTexBufferRange)(GLenum, GLenum, GLuint, GLintptr, GLsizeiptr);
ABI void   (*glBufferSubData)(GLenum, GLintptr, GLsizeiptr, const GLvoid *);
ABI void   (*glBufferStorage)(GLenum, GLsizeiptr, const GLvoid *, GLbitfield);

void gl_init_procs(void);

#endif