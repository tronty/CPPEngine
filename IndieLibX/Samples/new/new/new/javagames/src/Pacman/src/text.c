#include "text.h"

#include "imageloader.h"
#include "imageutil.h"

//Gets the image for the specified character.
static SDL_Surface *get_surface(char c);

//Gets the appropriate SDL_Color object for the given color.
static SDL_Color get_color(TextColor color);

//Sets the image to the given color.
static void set_fontimage_color(SDL_Surface *image, TextColor color);

static TextColor textColor;

static SDL_Surface *char_fonts[NUM_FONTS][NUM_CHAR_IMAGES];
static SDL_Surface *num_fonts [NUM_FONTS][NUM_NUM_IMAGES];
static SDL_Surface *spec_fonts[NUM_FONTS][NUM_SPEC_IMAGES];

void load_text(void)
{
	//imageloader.c has already loaded 1 copy of a white coloured font
	//clone the image for each of our font colours, then set the white to that font
	//let imageloader dispose of the original images, and make sure we dipose of ours in the dispose function

	textColor = WhiteText;

	for (int i = 0; i < NUM_CHAR_IMAGES; i++)
	{
		for (int c = 0; c < NUM_FONTS; c++)
		{
			char_fonts[c][i] = clone_image(get_char_images()[i]);
			set_fontimage_color(char_fonts[c][i], c);
		}
	}

	for (int i = 0; i < NUM_NUM_IMAGES; i++)
	{
		for (int c = 0; c < NUM_FONTS; c++)
		{
			num_fonts[c][i] = clone_image(get_num_images()[i]);
			set_fontimage_color(num_fonts[c][i], c);
		}
	}

	for (int i = 0; i < NUM_SPEC_IMAGES; i++)
	{
		for (int c = 0; c < NUM_FONTS; c++)
		{
			spec_fonts[c][i] = clone_image(get_spec_images()[i]);
			set_fontimage_color(spec_fonts[c][i], c);
		}
	}
}

void set_text_color(TextColor col)
{
	textColor = col;
}

bool is_valid(char c)
{
	bool isChar = c >= 'A' && c <= 'Z';
	bool isNum  = c >= '0' && c <= '9';
	bool isSpec = c == ' ' || c == '!' || c == '.' || c == '-' || c == '/' || c == '@' || c == '"';

	return isChar || isNum || isSpec;
}

bool is_valid_string(const char *text)
{
	const char *str = text;
	char c;

	while ((c = *text++))
	{
		if (!is_valid(c))
		{
			printf("invalid char: '%c' in string \"%s\"\n", c, str);
			return false;
		}
	}

	return true;
}

void dispose_text(void)
{
	for (int i = 0; i < NUM_CHAR_IMAGES; i++)
	{
		for (int c = 0; c < NUM_FONTS; c++)
		{
			SDL_FreeSurface(char_fonts[c][i]);
		}
	}

	for (int i = 0; i < NUM_NUM_IMAGES; i++)
	{
		for (int c = 0; c < NUM_FONTS; c++)
		{
			SDL_FreeSurface(num_fonts[c][i]);
		}
	}

	for (int i = 0; i < NUM_SPEC_IMAGES; i++)
	{
		for (int c = 0; c < NUM_FONTS; c++)
		{
			SDL_FreeSurface(spec_fonts[c][i]);
		}
	}
}

void draw_text(SDL_Surface *surface, const char *text, int x, int y)
{
	if (!is_valid_string(text))
	{
		printf("The string contains invalid characters: %s\n", text);
		printf("aborting\n");
		exit(1);
	}

	SDL_Rect location;
	location.x = x;
	location.y = y;

	char c;

	while ((c = *text++))
	{
		SDL_Surface *charSurface = get_surface(c);

		SDL_BlitSurface(charSurface, NULL, surface, &location);
		location.x = location.x + charSurface->w;
	}
}

void draw_numtext_coord(SDL_Surface *surface, const char *text, int x, int y)
{
	//a text square is 2 actual pixels out of alignment with number squares
	//do an offset when drawing
	draw_text(surface, text, x * 16, y * 16 - 2);
}

void draw_text_coord(SDL_Surface *surface, const char *text, int x, int y)
{
	draw_text(surface, text, x * 16, y * 16);
}

static void set_fontimage_color(SDL_Surface *image, TextColor color)
{
	replace_pixel(image, get_color(color));
}

static SDL_Surface *get_surface(char c)
{
	//if char, get the chara image
	if ('A' <= c && c <= 'Z') return char_fonts[textColor][c - 'A'];

	//if number, get the number image
	if ('0' <= c && c <= '9') return num_fonts[textColor][c - '0'];

	//if special, get the special image
	switch (c)
	{
		case ' ': return spec_fonts[textColor][0];
		case '.': return spec_fonts[textColor][1];
		case '-': return spec_fonts[textColor][2];
		case '@': return spec_fonts[textColor][3];
		case '!': return spec_fonts[textColor][4];
		case '/': return spec_fonts[textColor][5];
		case '"': return spec_fonts[textColor][6];
	}

	printf("error getting text surface with char %c\n", c);
	printf("aborting\n");
	exit(1);
}

static SDL_Color get_color(TextColor color)
{
	switch (color)
	{
		case WhiteText:  return (SDL_Color) { 223, 223, 255, 255 };
		case YellowText: return (SDL_Color) { 255, 255,   0, 255 };
		case PeachText:  return (SDL_Color) { 255, 185, 175, 255 };
		case RedText:    return (SDL_Color) { 255,   0,   0, 255 };
		case PinkText:   return (SDL_Color) { 255, 185, 255, 255 };
		case CyanText:   return (SDL_Color) {   0, 255, 255, 255 };
		case OrangeText: return (SDL_Color) { 255, 185,  81, 255 };
	}

	printf("error color: %d\naborting\n", color);
	exit(1);
}
