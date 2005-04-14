/*  DreamChess
 *  Copyright (C) 2005  The DreamChess project
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#include "SDL.h"
#include "SDL_ttf.h"
#include "png.h"

static void abort_(const char * s, ...)
{
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    fprintf(stderr, "\n");
    va_end(args);
    abort();
}

static void generate_text_char(TTF_Font *font, unsigned char letter,
                               SDL_Surface *image, int x, int y )
{
    SDL_Surface *text_texture;
    SDL_Color black = {255,255,255,0};
    SDL_Rect rect;
    char string[2];

    string[0] = letter;
    string[1] = '\0';

    if (!(text_texture=TTF_RenderText_Blended(font, string, black)))
        abort_("[generate_text_char] %s", TTF_GetError());

    SDL_SetAlpha(text_texture, 0, 0);

    rect.x = x;
    rect.y = y;

    SDL_BlitSurface(text_texture, NULL, image, &rect);

    SDL_FreeSurface( text_texture );
}

static int line_width(char *w, int n)
{
    int i, size = 0;
    for (i = 0; i < n; i++)
        size += w[i];
    return size;
}

static int char_width(TTF_Font *font, unsigned char c)
{
    unsigned char s[2];
    int w, h;
    s[1] = '\0';
    s[0] = c;
    TTF_SizeText(font, s, &w, &h);
    return w;
}

void write_png_file(int width, int height, char* file_name, png_bytep *rows)
{
    png_structp png_ptr;
    png_infop info_ptr;

    /* create file */
    FILE *fp = fopen(file_name, "wb");
    if (!fp)
        abort_("[write_png_file] File %s could not be opened for writing",
               file_name);


    /* initialize stuff */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,
                                      NULL);

    if (!png_ptr)
        abort_("[write_png_file] png_create_write_struct failed");

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
        abort_("[write_png_file] png_create_info_struct failed");

    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[write_png_file] Error during init_io");

    png_init_io(png_ptr, fp);


    /* write header */
    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[write_png_file] Error during writing header");

    png_set_IHDR(png_ptr, info_ptr, width, height,
                 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    /* write bytes */
    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[write_png_file] Error during writing bytes");

    png_write_image(png_ptr, rows);


    /* end write */
    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[write_png_file] Error during end of write");

    png_write_end(png_ptr, NULL);

}

/* Your program's main entry point */
int main(int argc, char **argv)
{
    unsigned char table[256];
    unsigned char width[256];
    int i, j, max = 0, height;
    FILE *f;
    png_bytep * row_pointers;
    TTF_Font *font;
    SDL_Surface *image;


    if (argc < 4)
    {
        printf("Usage: ttf2png <input.ttf> <output.png> <output.wid>\n");
        exit(0);
    }

    if (TTF_Init() == -1)
        abort_("[main] %s", TTF_GetError());

    font = TTF_OpenFont(argv[1], 16);

    if (!font)
        abort_("[main] %s", TTF_GetError());

    table[0] = 1;
    width[0] = char_width(font, 1);
    for (i = 1; i < 256; i++)
    {
        table[i] = i;
        width[i] = char_width(font, i);
    }

    for (i = 0; i < 16; i++)
    {
        int len;
        len = line_width(&width[i*16], 16);
        if (len > max)
            max = len;
    }

    height = TTF_FontHeight(font);

    image = SDL_CreateRGBSurface(
                SDL_SWSURFACE,
                max, height*16,
                32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
                0x000000FF,
                0x0000FF00,
                0x00FF0000,
                0xFF000000
#else
                0xFF000000,
                0x00FF0000,
                0x0000FF00,
                0x000000FF
#endif
            );

    if (!image)
        abort_("[main] Couldn't create RGB surface");

    for (i = 0; i < 16; i++)
    {
        int x = 0;
        for (j = 0; j < 16; j++)
        {
            generate_text_char(font, table[i*16+j], image, x, i*height);
            x += width[i*16+j];
        }
    }

    row_pointers = malloc(height * 16 * sizeof(png_bytep));

    if (!row_pointers)
        abort_("[main] Out of memory");

    for (i = 0; i < height*16; i++)
        row_pointers[i] = image->pixels+(i * image->pitch);

    write_png_file(max, height*16, argv[2], row_pointers);

    f = fopen(argv[3], "wb");

    if (!f)
        abort_("[main] File %s could not be opened for writing", argv[3]);

    if (fwrite(width, 1, 256, f) < 256)
        abort_("[main] Error writing to %s", argv[3]);

    fclose(f);

    TTF_CloseFont(font);
    return 0;
}
