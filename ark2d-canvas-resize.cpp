/*
 * Copyright (c) 2017 Ashley Gwinnell <ashley@forceofhab.it>
 *
 * ark2d-canvas-resize is free software. 
 *
 * MIT licence.
 */

#include <cstdio>
#include <cstdint>
#include <cmath>

#include "SDL.h"
#include "SDL_image.h"

inline Uint32 SDL_GetPixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

inline void SDL_PutPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}


SDL_Surface* createARGBSurface(int w, int h) {
	return SDL_CreateRGBSurface(0, w, h, 32, 0xff0000U, 0xff00U, 0xffU, 0xff000000U);
}
int main(int argc, char* argv[]) {
	if (argc != 8) {
		fprintf(stderr, "usage: ark2d-canvas-resize pixels_x pixels_y align_x align_y relative input_image output_image\n");
		fprintf(stderr, "pixels_x/y must be a number\n");
		fprintf(stderr, "align_x/aligny must be -1 / 0 / 1 \n");
		fprintf(stderr, "relative must be true/false string \n");
		return 1;
	}
	
	int expand_pixels_x = atoi(argv[1]);
	int expand_pixels_y = atoi(argv[2]);
	int align_x = atoi(argv[3]);
	int align_y = atoi(argv[4]);
	char* relative_str = argv[5];
	bool relative = (strcmp(relative_str, "true") == 0);
	char* in_file = argv[6];
	char* out_file = argv[7];
	
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
		return 1;
	}
	
	SDL_Surface* src_img = IMG_Load(in_file);
	if (!src_img) {
		fprintf(stderr, "Failed to load source image '%s': %s\n", in_file, IMG_GetError());
		return 1;
	}
	
	int src_width = src_img->w;
	int src_height = src_img->h;
	int dst_width = 0;
	int dst_height = 0;

	int pixel_offset_x = 0;
	int pixel_offset_y = 0;

	if (relative) { 
		dst_width = src_width + expand_pixels_x;
		dst_height = src_height + expand_pixels_y;
		if (expand_pixels_x <= src_width *-1) {
		 	fprintf(stderr, "pixels_x must be between greater than the negative width when relative=true %i\n", expand_pixels_x);
		 	return 1;
		}
		if (expand_pixels_y <= src_height * -1 ) {
			fprintf(stderr, "pixels_y must be between greater than the negative height when relative=true %i\n", expand_pixels_y);
		 	return 1;	
		}

		if (align_x == 0) {
			pixel_offset_x = expand_pixels_x/2;
		} else if (align_x == 1) {
			pixel_offset_x = expand_pixels_x;
		}

		if (align_y == 0) {
			pixel_offset_y = expand_pixels_y/2;
		} else if (align_y == 1) {
			pixel_offset_y = expand_pixels_y;
		}

	} else {
		dst_width = expand_pixels_x;
		dst_height = expand_pixels_y;
		if (dst_width <= 0 || dst_height <= 0) {
			fprintf(stderr, "pixels_x and pixels_y must be between greater 0\n");
		 	return 1;	
		}

		if (align_x == 0) {
			pixel_offset_x = (dst_width - src_width)/2;
		} else if (align_x == 1) {
			pixel_offset_x = dst_width - src_width;
		}

		if (align_y == 0) {
			pixel_offset_y = (dst_height - src_height)/2;
		} else if (align_y == 1) {
			pixel_offset_y = dst_height - src_height;
		}
	}

	// printf("new size %d x %d...\n", dst_width, dst_height);

	int x, y, offset;
	Uint8 r, g, b, a;

	uint32_t *in_data = new uint32_t[src_img->w * src_img->h];
	offset = 0;
	for(y = 0; y < src_img->h; y++) {
		for(x = 0; x < src_img->w; x++) {
			uint32_t c = SDL_GetPixel(src_img, x, y);
			SDL_GetRGBA(c, src_img->format, &r, &g, &b, &a);
			//fprintf(stdout, "%02x%02x%02x%02x ", a, r, g, b);
			in_data[offset] = (
						(uint32_t(a) << 24)
						| (uint32_t(r) << 16)
						| (uint32_t(g) << 8)
						| (uint32_t(b))
					);
//			fprintf(stdout, "%08x ", in_data[offset]);
			offset++;
		}
//		fprintf(stdout, "\n");
	}
	SDL_FreeSurface(src_img);

	
	// printf("Expanding/shrinking image...\n");
	uint32_t* dest = new uint32_t[dst_width * dst_height];
	
	// printf("Saving image...\n");
	SDL_Surface* dst_img = createARGBSurface(dst_width, dst_height);
	if (!dst_img) {
		delete [] dest;
		fprintf(stderr, "Failed to create SDL surface: %s\n", SDL_GetError());
		return 1;
	}

	offset = 0;
	for(y = 0; y < src_height; y++) {
		for(x = 0; x < src_width; x++) {
			a = ((in_data[offset] >> 24) & 0xff);
			r = (in_data[offset] >> 16) & 0xff;
			g = (in_data[offset] >> 8) & 0xff;
			b = (in_data[offset]) & 0xff;
			uint32_t c = SDL_MapRGBA(dst_img->format, r, g, b, a);
//			fprintf(stdout, "%02x%02x%02x%02x ", a, r, g, b);
//			fprintf(stdout, "%08x ", c);
//			fprintf(stdout, "%08x ", dest[offset]);
			int newpixelx = x + pixel_offset_x;
			int newpixely = y + pixel_offset_y;
			if (newpixelx >= 0 && newpixely >= 0 && newpixelx < dst_width && newpixely < dst_height) { 
				SDL_PutPixel(dst_img, newpixelx, newpixely, c);
			}
			//SDL_PutPixel(dst_img, x, y, (a || r || g || b) ? 0xffffffffU : 0);
			offset++;
		}
	}

	IMG_SavePNG(dst_img,out_file);
	
	SDL_FreeSurface(dst_img);
	
	SDL_Quit();

	delete [] dest;
	delete [] in_data;

	return 0;
}
