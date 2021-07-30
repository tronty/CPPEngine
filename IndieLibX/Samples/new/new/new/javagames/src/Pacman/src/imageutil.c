#include "imageutil.h"

#include <SDL/SDL_gfxPrimitives.h>

SDL_Surface* clone_image(SDL_Surface* src)
{
	return SDL_ConvertSurface(src, src->format, SDL_SWSURFACE);
}

void replace_pixel(SDL_Surface *image, SDL_Color newColor)
{
	for (int y = 0; y < image->h; y++)
	{
		for (int x = 0; x < image->w; x++)
		{
			Uint32 pixel = get_pixel(image, x, y);
			SDL_Color c;
			SDL_GetRGBA(pixel, image->format, &c.r, &c.g, &c.b, &c.unused);

			if (c.unused == 255) pixelRGBA(image, x, y, newColor.r, newColor.g, newColor.b, newColor.unused);
		}
	}
}

Uint32 get_pixel(SDL_Surface *surface, int x, int y)
{
	int bpp = surface->format->BytesPerPixel;
	//Here p is the address to the pixel we want to retrieve
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch(bpp) {
	case 1:
		return *p;
		break;

	case 2:
		return *(Uint16 *)p;
		break;

	case 3:
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
		break;

	case 4:
		return *(Uint32 *)p;
		break;

	default:
		//Shouldn't happen, but avoids warnings
		return 0;
	}
}
