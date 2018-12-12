#ifndef M3D_H
#define M3D_H

typedef struct m3d m3d;

struct m3d *m3d_new(const char *path);
void m3d_free(struct m3d *m);
struct img *m3d_get_tex(struct m3d *m, int n);
float *m3d_get_verts(struct m3d *m, int n, uint32_t *out);
unsigned short *m3d_get_elems(struct m3d *m, int n, uint32_t *out);

#endif