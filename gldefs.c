#include <windows.h>
/* We want to define the symbols, not declare them */
#define _GL_NO_EXTERN
#include "gldefs.h"

void gl_init_procs(void)
{
	glBindBuffer = (void *)wglGetProcAddress("glBindBuffer");
	glGenBuffers = (void *)wglGetProcAddress("glGenBuffers");
	glBufferData = (void *)wglGetProcAddress("glBufferData");
	glGenVertexArrays = (void *)wglGetProcAddress("glGenVertexArrays");
	glBindVertexArray = (void *)wglGetProcAddress("glBindVertexArray");
	glVertexAttribPointer = (void *)wglGetProcAddress("glVertexAttribPointer");
	glEnableVertexAttribArray = (void *)wglGetProcAddress("glEnableVertexAttribArray");
	glCreateShader = (void *)wglGetProcAddress("glCreateShader");
	glShaderSource = (void *)wglGetProcAddress("glShaderSource");
	glCompileShader = (void *)wglGetProcAddress("glCompileShader");
	glCreateProgram = (void *)wglGetProcAddress("glCreateProgram");
	glUseProgram = (void *)wglGetProcAddress("glUseProgram");
	glLinkProgram = (void *)wglGetProcAddress("glLinkProgram");
	glAttachShader = (void *)wglGetProcAddress("glAttachShader");
	wglSwapIntervalEXT = (void *)wglGetProcAddress("wglSwapIntervalEXT");
	glGetUniformLocation = (void *)wglGetProcAddress("glGetUniformLocation");
	glUniform1ui = (void *)wglGetProcAddress("glUniform1ui");
	glUniform1i = (void *)wglGetProcAddress("glUniform1i");
	glDeleteShader = (void *)wglGetProcAddress("glDeleteShader");
	glDeleteProgram = (void *)wglGetProcAddress("glDeleteProgram");
	glActiveTexture = (void *)wglGetProcAddress("glActiveTexture");
	glGenerateMipmap = (void *)wglGetProcAddress("glGenerateMipmap");
	glGetShaderInfoLog = (void *)wglGetProcAddress("glGetShaderInfoLog");
	glGetShaderiv = (void *)wglGetProcAddress("glGetShaderiv");
	glUniform1uiv = (void *)wglGetProcAddress("glUniform1uiv");
	glUniform1iv = (void *)wglGetProcAddress("glUniform1uiv");
	glTexBufferRange = (void *)wglGetProcAddress("glTexBufferRange");
	glBufferSubData = (void *)wglGetProcAddress("glBufferSubData");
	glBufferStorage = (void *)wglGetProcAddress("glBufferStorage");
}
