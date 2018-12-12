#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <png.h>
#include "log.h"
#include "img.h"

#define zerror nlog

void png_kill(struct img *p)
{
	p->w = 0;
	p->h = 0;
	free(p->pixels);
}

struct img png_load(const char *name)
{
	unsigned char header[8];
	FILE *f;
	struct img p = {0};
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned char **rows;
	int32_t depth, type;

	f = fopen(name, "rb");
	if(!f)
	{
		zerror("Unable to open png: %s", name);
		goto out;
	}

	if(fread(header, 1, 8, f) != 8)
	{
		zerror("Unable to read png header");
		goto out;
	}

	if(png_sig_cmp(header, 0, 8))
	{
		zerror("Malformed png file");
		goto out;
	}

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png_ptr)
	{
		zerror("Failed to init libpng");
		goto out;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr)
	{
		zerror("Failed to init libpng info struct");
		goto out;
	}

	if(setjmp(png_jmpbuf(png_ptr)))
	{
		zerror("longjmp");
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		goto out;
	}

	png_init_io(png_ptr, f);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &p.w, &p.h, &depth, &type, NULL, NULL, NULL);

	if(depth != 8)
	{
		zerror("Image has unsupported bitdepth");
		goto out;
	}

	if(type == PNG_COLOR_TYPE_RGB)
		p.format = 3;
	else if(type == PNG_COLOR_TYPE_RGB_ALPHA)
		p.format = 4;
	else
	{
		zerror("PNG type is not RGBA or RGBA");
		goto out;
	}

	p.pixels = malloc(p.w * p.h * p.format);
	rows = malloc(sizeof(void *) * p.h);
	for(size_t i = 0; i < p.h; i++)
		rows[p.h-i-1] = &((unsigned char *)p.pixels)[p.w * p.format * i];

	png_read_image(png_ptr, rows);
	png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
out:
	fclose(f);
	if(rows)
		free(rows);
	return p;
}

#ifdef TEST
int main(void)
{
	struct img p;
	p = load_png("test.png");
	if(!p.pixels)
		return EXIT_FAILURE;

	printf("Loaded the test png.\n");
	return EXIT_SUCCESS;
}
#endif