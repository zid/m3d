#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gl/gl.h>
#include "shader.h"
#include "log.h"
#include "gldefs.h"

static char *load_txt(const char *name)
{
	FILE *f;
	size_t len;
	char *s;

	f = fopen(name, "rb");
	if(!f)
		return NULL;

	fseek(f, 0, SEEK_END);
	len = ftell(f);
	rewind(f);

	s = malloc(len+1);
	fread(s, 1, len, f);

	s[len] = 0;

	fclose(f);

	return s;
}

struct shader *shader_new(const char *vpath, const char *fpath)
{
	struct shader *s;
	char *vshade_src, *fshade_src, *buf;
	GLuint vshade, fshade, shader;
	GLint len;

	vshade_src = load_txt(vpath);

	vshade = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vshade, 1, (const char **)&vshade_src, NULL);
	glCompileShader(vshade);
	free(vshade_src);
	glGetShaderiv(vshade, GL_INFO_LOG_LENGTH, &len);
	buf = malloc(len);
	glGetShaderInfoLog(vshade, len, NULL, buf);
	if(len)
		nlog(buf);
	free(buf);

	fshade_src = load_txt(fpath);

	fshade = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshade, 1, (const char **)&fshade_src, NULL);
	glCompileShader(fshade);
	free(fshade_src);
	glGetShaderiv(fshade, GL_INFO_LOG_LENGTH, &len);
	buf = malloc(len);
	glGetShaderInfoLog(fshade, len, NULL, buf);
	if(len)
		nlog(buf);
	free(buf);

	shader = glCreateProgram();
	glAttachShader(shader, vshade);
	glAttachShader(shader, fshade);
	glLinkProgram(shader);

	s = malloc(sizeof (struct shader));

	s->vshader_path = strdup(vpath);
	s->fshader_path = strdup(fpath);
	s->program = shader;
	s->vshader = vshade;
	s->fshader = fshade;

	return s;
}

void shader_destroy(struct shader *s)
{
	glDeleteShader(s->vshader);
	glDeleteShader(s->fshader);
	glDeleteProgram(s->program);

	free((void *)s->vshader_path);
	free((void *)s->fshader_path);

	free(s);
}

/* This makes an extra copy of the file paths, but not
 * doing it would need adding extra API functions. */
void shader_reload(struct shader *s)
{
	const char *fpath, *vpath;

	/* These are about to get wiped out, keep a copy */
	fpath = strdup(s->fshader_path);
	vpath = strdup(s->vshader_path);

	/* Clean up the old shaders */
	shader_destroy(s);

	/* Reload the files and make new ones */
	s = shader_new(fpath, vpath);

	/* Free the temporary copies we held so we could delete the old shader */
	free((void *)fpath);
	free((void *)vpath);
}