#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "m3d.h"
#include "log.h"
#include "img.h"

#define TEX_SIZE 0x40
#define ENT_SIZE 0xE0
#define OBJ_SIZE 0xB0
#define STRIP_SIZE 12

enum
{
	M3D_OK,
	M3D_ERR_TEX,
	M3D_ERR_MAT,
	M3D_ERR_ENT,
	M3D_ERR_OBJ
};

struct m3d_uv
{
	union
	{
		float uv[2];
		struct
		{
			float u, v;
		};
	};
};

struct m3d_pos
{
	union
	{
		float xyz[3];
		struct
		{
			float x, y, z;
		};
	};
};

struct m3d_vert
{
	struct m3d_pos p;
	struct m3d_uv uv;
};

struct m3d_elems
{
	uint32_t elem_count;
	unsigned short *elems;
};

struct m3d_obj
{
	uint32_t vert_count;
	uint32_t  idx_count;

	struct m3d_vert *vert;
	struct m3d_elems elem;
	uint16_t *idx;
};

struct m3d_ent
{
	uint32_t obj_count;
	struct m3d_obj **obj;
};

struct m3d_tex
{
	struct img i;
	char name[64];
};

struct m3d
{
	uint32_t tex_count;
	uint32_t ent_count;

	struct m3d_ent **ent;
	struct m3d_tex **tex;
};

static uint32_t u32_at(const unsigned char *b)
{
	uint32_t v;

	v = b[0] | b[1]<<8 | b[2]<<16 | b[3]<<24;

	return v;
}

static uint16_t u16_at(const unsigned char *b)
{
	uint16_t v;

	v = b[0] | b[1]<<8;

	return v;
}

/* Turns GL_TRIANGLE_STRIP entries into a big GL_TRIANGLE array */
static int m3d_process_strips(const uint8_t *buf, struct m3d_obj *obj, uint32_t strip_count)
{
	uint32_t i, j;
	uint32_t elem_count = 0;
	uint16_t *elems;
	uint16_t v0, v1, v2;

	/* Need to pre-count the number of elements in the strips
	 * so the correct amount of space can be allocated. */
	for(i = 0; i < strip_count; i++)
	{
		uint32_t strip_size;

		strip_size = u32_at(&buf[i*STRIP_SIZE]);
		elem_count += (strip_size-2) * 3;
	}

	elems = malloc(elem_count * sizeof (short));
	obj->elem.elems = elems;
	obj->elem.elem_count = elem_count;

	/*
	 * For each strip, grab the number of triangles in the strip and
	 * where it starts within the index array.
	 * Alternate triangles need to have their element order rotated so that
	 * the triangles stay wound clockwise.
	 */
	for(i = 0; i < strip_count; i++)
	{
		uint32_t off, count;

		count = u32_at(&buf[i * STRIP_SIZE]);
		off   = u32_at(&buf[i * STRIP_SIZE + 8]);

		if(off + count > obj->idx_count)
			return M3D_ERR_OBJ;

		v0 = obj->idx[off];
		v1 = obj->idx[off+1];

		for(j = 0; j < count-2; j++)
		{
			v2 = obj->idx[off+j+2];

			if((j & 1) == 0)
			{
				*elems++ = v1;
				*elems++ = v0;
				*elems++ = v2;
			}
			else
			{
				*elems++ = v0;
				*elems++ = v1;
				*elems++ = v2;
			}
			v0 = v1;
			v1 = v2;
		}
	}

	return M3D_OK;
}

static void m3d_load_idxs(const uint8_t *buf, uint32_t idx_count, uint16_t *idx)
{
	uint32_t i;

	for(i = 0; i < idx_count; i++)
		idx[i] = u16_at(&buf[i*2]);
}

/*
 * Read a float[3] and a float[2] from the 'pos' and 'uv'
 * offset respectively and produce an output float[5].
 *
 * Assumes floats are in host format.
 */
static void m3d_load_verts(const uint8_t *buf, uint32_t vert_count,
	uint32_t pos_offset, uint32_t uvs_offset, struct m3d_vert *v)
{
	uint32_t i;

	for(i = 0; i < vert_count; i++)
	{
		float *p;

		p = (float *)(buf + pos_offset + i * sizeof (float[3]));
		v->p.x = p[0];
		v->p.y = p[1];
		v->p.z = p[2];

		p = (float *)(buf + uvs_offset + i * sizeof (float[2]));
		v->uv.u = p[0];
		v->uv.v = p[1];
		v++;
	}
}

/*
 * obj entries contain information about triangle strips,
 * vertex data (xyz, uv) and the indicies used by the strips
 */
static int m3d_load_obj(const uint8_t *buf,
	uint32_t obj_off, struct m3d_obj *obj, uint32_t len)
{
	uint32_t idx_offset;
	uint32_t pos_offset;
	uint32_t uvs_offset;
	uint32_t strip_offset, strip_count;

	obj->idx_count  = u32_at(&buf[obj_off + 0x50]);
	obj->vert_count = u32_at(&buf[obj_off + 0x6C]);

	idx_offset = u32_at(&buf[obj_off + 0x58]);
	pos_offset = u32_at(&buf[obj_off + 0x80]);
	uvs_offset = u32_at(&buf[obj_off + 0x74]);

	strip_offset = u32_at(&buf[obj_off + 0x64]);
	strip_count  = u32_at(&buf[obj_off + 0x5C]);

	if(obj->idx_count > 65535
		|| obj->idx_count *  sizeof(short[2]) + idx_offset > len
		|| obj->vert_count * sizeof(float[3]) + pos_offset > len
		|| obj->vert_count * sizeof(float[2]) + uvs_offset > len
		|| strip_count * STRIP_SIZE + strip_offset > len
	)
		return M3D_ERR_OBJ;

	obj->vert = malloc(sizeof (struct m3d_vert) * obj->vert_count);
	obj->idx  = malloc(sizeof (uint16_t) * obj->idx_count);

	m3d_load_verts(buf, obj->vert_count, pos_offset, uvs_offset, obj->vert);
	m3d_load_idxs(&buf[idx_offset], obj->idx_count, obj->idx);
	m3d_process_strips(&buf[strip_offset], obj, strip_count);

	return M3D_OK;
}

/* Entry seems to just contain the amount of objects in each entry */
static int m3d_load_entry(const uint8_t *buf,
	uint32_t ent_off, struct m3d_ent *ent, uint32_t len)
{
	uint32_t i, j, obj_offset;
	int r;

	ent->obj_count  = u32_at(&buf[ent_off + 0x80]);
	obj_offset = u32_at(&buf[ent_off + 0x88]);

	if(obj_offset + ent->obj_count * OBJ_SIZE > len)
		return M3D_ERR_ENT;

	ent->obj = malloc(sizeof (struct m3d_obj *) * ent->obj_count);
	for(i = 0; i < ent->obj_count; i++)
	{
		uint32_t off;

		ent->obj[i] = malloc(sizeof (struct m3d_obj));
		off = OBJ_SIZE * i + obj_offset;
		r = m3d_load_obj(buf, off, ent->obj[i], len);
		if(r != M3D_OK)
			goto err;
	}

	return M3D_OK;

err:
	for(j = 0; j <= i; i++)
		free(ent->obj[i]);
	free(ent->obj);
	return r;
}

static struct m3d_ent **m3d_load_entries(const uint8_t *buf,
	uint32_t ent_offset, uint32_t ent_count, uint32_t len)
{
	struct m3d_ent **ent;
	uint32_t i, j;
	int r;

	if(ent_count <= 0)
		return NULL;

	ent = calloc(sizeof (struct m3d_ent *), ent_count);
	for(i = 0; i < ent_count; i++)
	{
		uint32_t off;

		ent[i] = malloc(sizeof (struct m3d_ent));
		off = ent_offset + OBJ_SIZE * i;

		r = m3d_load_entry(buf, off, ent[i], len);
		if(r != M3D_OK)
			goto err;
	}

	return ent;

err:
	for(j = 0; j <= i; j++)
		free(ent[i]);
	free(ent);
	return NULL;
}

/* I don't have a .tga loader handy so I just pretend it contains
 * references to PNG files. */
static void m3d_load_texture(struct m3d_tex *t)
{
	size_t len;
	char buf[64];

	len = strlen(t->name);
	memcpy(buf, t->name, len+1);

	/* Disregard the original targas, pretend it wanted pngs */
	if(len > 4 && strcasecmp(&buf[len-4], ".tga") == 0)
		memcpy(&buf[len-4], ".png", 4);

	t->i = png_load(buf);
}

static struct m3d_tex **m3d_load_textures(const uint8_t *buf,
	uint32_t tex_count)
{
	struct m3d_tex **tex;
	uint32_t i;

	if(tex_count <= 0)
		return NULL;

	tex = malloc(sizeof (struct m3d_tex *) * tex_count);
	for(i = 0; i < tex_count; i++)
	{
		uint32_t off;

		tex[i] = malloc(sizeof (struct m3d_tex));

		off = TEX_SIZE * i;
		memcpy(tex[i]->name, &buf[off], 63);
		tex[i]->name[63] = '\0';

		m3d_load_texture(tex[i]);
	}

	return tex;
}

/* dframe01 seems to contain references to materials, but I don't need to figure them out */
static int m3d_parse(const unsigned char *buf, uint32_t len, struct m3d **mout)
{
	struct m3d *m;
	uint32_t tex_offset, ent_offset;
	int r;

	m = malloc(sizeof (struct m3d));
	m->tex_count  = u32_at(&buf[0x100]);
	/* m->mat_count  = u32_at(&buf[0x108]); */
	m->ent_count  = u32_at(&buf[0x110]);

	tex_offset = u32_at(&buf[0x118]);
	/* m->mat_offset = u32_at(&buf[0x11C]); */
	ent_offset = u32_at(&buf[0x120]);

	/* Don't load things that are obscenely large, or
	 * are malformed regarding field offsets */
	if(m->tex_count > 65535
		|| m->tex_count * TEX_SIZE + tex_offset > len
	)
	{
		r = M3D_ERR_TEX;
		goto err;
	}

	/*if(m->mat_count > 65535)
		return M3D_ERR_MAT;*/

	if(m->ent_count > 65535
		|| m->ent_count * ENT_SIZE + ent_offset > len
	)
		r = M3D_ERR_ENT;

	m->tex = m3d_load_textures(&buf[tex_offset], m->tex_count);
	if(!m->tex)
	{
		r = M3D_ERR_TEX;
		goto err;
	}

	m->ent = m3d_load_entries(buf, ent_offset, m->ent_count, len);
	if(!m->ent)
	{
		r = M3D_ERR_ENT;
		goto err;
	}

	*mout = m;

	return M3D_OK;
	
err:
	m3d_free(m);
	return r;
}

struct m3d *m3d_new(const char *path)
{
	FILE *f;
	size_t len;
	unsigned char *buf;
	struct m3d *m = {0};

	f = fopen(path, "rb");
	if(!f)
		return NULL;

	fseek(f, 0, SEEK_END);
	len = ftell(f);
	fseek(f, 0, SEEK_SET);

	buf = malloc(len);
	if(!buf)
		goto out;

	fread(buf, len, 1, f);

	if(m3d_parse(buf, len, &m) != M3D_OK)
		goto out2;

	return m;

out2:
	free(buf);
out:
	fclose(f);
	return NULL;
}

static void m3d_free_obj(struct m3d_obj *o)
{
	free(o->vert);
	free(o->idx);
	free(o->elem.elems);
	free(o);
}

static void m3d_free_ent(struct m3d *m)
{
	uint32_t i;

	for(i = 0; i < m->ent_count; i++)
	{
		uint32_t j;

		for(j = 0; j < m->ent[i]->obj_count; j++)
			m3d_free_obj(m->ent[i]->obj[j]);

		free(m->ent[i]->obj);
		free(m->ent[i]);
	}
	free(m->ent);
}

static void m3d_free_tex(struct m3d *m)
{
	uint32_t i;

	for(i = 0; i < m->tex_count; i++)
	{
		free(m->tex[i]);
	}

	free(m->tex);
}

void m3d_free(struct m3d *m)
{
	if(m->tex)
		m3d_free_tex(m);

	if(m->ent)
		m3d_free_ent(m);

	free(m);
}

float *m3d_get_verts(struct m3d *m, int n, uint32_t *out)
{
	*out = m->ent[0]->obj[n]->vert_count;
	return &m->ent[0]->obj[n]->vert->p.x;
}

unsigned short *m3d_get_elems(struct m3d *m, int n, uint32_t *out)
{
	*out = m->ent[0]->obj[n]->elem.elem_count;
	return m->ent[0]->obj[n]->elem.elems;
}

struct img *m3d_get_tex(struct m3d *m, int n)
{
	return &m->tex[n]->i;
}