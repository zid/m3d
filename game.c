#include <gl/gl.h>
#include "gldefs.h"
#include "img.h"
#include "shader.h"
#include "game.h"
#include "m3d.h"
#include "log.h"

struct model
{
	float *up_f, *lo_f;
	unsigned short *up_s, *lo_s;

	uint32_t up_elems, up_verts;

	GLuint program;
	GLuint vao, up_vbo[2], tex[2];

	struct shader *shader;
};


static struct model frame;


void game_reload()
{
	shader_reload(frame.shader);
}


void game_init()
{
	struct m3d *m;
	struct img *i;

	m = m3d_new("frame01.m3d");
	frame.shader = shader_new("vshade.txt", "fshade.txt");

	frame.up_f = m3d_get_verts(m, 0, &frame.up_verts);
	frame.up_s = m3d_get_elems(m, 0, &frame.up_elems);

	glGenVertexArrays(1, &frame.vao);
	glBindVertexArray(frame.vao);

	glGenBuffers(2, frame.up_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, frame.up_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof (float[5]) * frame.up_verts, frame.up_f, GL_STATIC_DRAW);
	/* [XYZ..] */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof (float[5]), 0);
	/* [...UV] */
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof (float[5]), (GLvoid *)(sizeof (float[3])));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frame.up_vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, frame.up_elems * sizeof (short), frame.up_s, GL_STATIC_DRAW);

	i = m3d_get_tex(m, 1);
	glGenTextures(1, frame.tex);
	glBindTexture(GL_TEXTURE_2D, frame.tex[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, i->w, i->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, i->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	m3d_free(m);

	glUseProgram(frame.shader->program);
	glEnable(GL_DEPTH_TEST);
}

void game()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, frame.up_elems, GL_UNSIGNED_SHORT, 0);
}

void game_die()
{

}
