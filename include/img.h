#ifndef IMG_H
#define IMG_H

#include <stdint.h>

struct img {
	uint32_t w, h;
	int format;
	void *pixels;
};

void png_kill(struct img *);
struct img png_load(const char *);

#endif