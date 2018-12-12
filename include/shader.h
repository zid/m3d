#ifndef SHADER_H
#define SHADER_H

#include <gl/gl.h>

struct shader
{
	GLuint program;
	GLuint vshader, fshader;
	const char *vshader_path, *fshader_path;
};

/* Creates a shader program from a pair of file paths */
void shader_destroy(struct shader *s);
struct shader *shader_new(const char *vpath, const char *fpath);
void shader_reload(struct shader *s);
#endif