/*  DreamChess
 *  Copyright (C) 2006  The DreamChess project
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

/* Convert 32x32 4 bpp indexed PNG image to Dreamcast BIOS icon. */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <png.h>

static png_structp png_ptr;
static png_infop info_ptr;
static png_bytep *rows;

static png_colorp palette;
static int num_palette;

static png_bytep trans;
static int num_trans;

#define PAL_ENTRIES 16
#define IMAGE_WIDTH 32
#define IMAGE_HEIGHT 32

static void abort_(const char * s, ...)
{
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    va_end(args);
    abort();
}

int to_argb4444(int r, int g, int b, int a)
{
    return ((a >> 4) << 12) | ((r >> 4) << 8) | ((g >> 4) << 4) | (b >> 4);
}

void write_palette(FILE *f)
{
    int i;

    fprintf(f, "char palette[%i] = {\n", PAL_ENTRIES);

    for (i = 0; i < num_palette; i++)
    {
        int argb = to_argb4444(palette[i].red, palette[i].green, palette[i].blue, i < num_trans ? trans[i] : 255);
        fprintf(f, "    0x%02x, 0x%02x", argb & 0xff, argb >> 8);
        if (i != num_palette - 1)
            fprintf(f, ", /* %2i */\n", i);
        else
            fprintf(f, "  /* %2i */\n};\n", i);
    }
}

void write_data(FILE *f)
{
    int i;

    fprintf(f, "char pixels[%i] = {\n", IMAGE_WIDTH * IMAGE_HEIGHT / 2);

    for (i = 0; i < IMAGE_HEIGHT; i++)
    {
        int j;

        fprintf(f, "    ");

        for (j = 0; j < IMAGE_WIDTH / 2; j++)
        {
            fprintf(f, "0x%02x", rows[i][j]);
            if (j == IMAGE_WIDTH / 4 - 1)
                fprintf(f, ",\n    ");
            else if (j != IMAGE_WIDTH / 2 - 1)
                fprintf(f, ", ");
        }

        if (i != IMAGE_HEIGHT - 1)
            fprintf(f, ",\n");
        else
            fprintf(f, "\n};\n");
    }
}

int main(int argc, char **argv)
{
    FILE *fin, *fout;

    if (argc < 3)
    {
        printf("Usage: dcicon <icon.png> <icon.c>\n");
        exit(0);
    }

    fin = fopen(argv[1], "rb");
    if (!fin)
        abort_("File %s could not be opened for reading", argv[1]);

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
        abort_("Failed to create PNG read struct\n");

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
        abort_("Failed to create PNG info struct\n");

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
        fclose(fin);

        abort_("Error reading PNG\n");
    }

    png_init_io(png_ptr, fin);
    png_read_png(png_ptr, info_ptr, 0, png_voidp_NULL);

    if ((png_get_image_width(png_ptr, info_ptr) != IMAGE_WIDTH) ||
        (png_get_image_height(png_ptr, info_ptr) != IMAGE_HEIGHT))
        abort_("Image is not %ix%i pixels\n", IMAGE_WIDTH, IMAGE_HEIGHT);

    if ((png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_PALETTE)
        || (png_get_bit_depth(png_ptr, info_ptr) != 4))
        abort_("Image is not a paletted image with bit depth 4\n");

    if (!png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette))
        abort_("Error reading palette\n");

    if (num_palette > PAL_ENTRIES)
        abort_("There are %i palette entries, which exceeds the maximum of %i\n", num_palette, PAL_ENTRIES);

    if (!png_get_tRNS(png_ptr, info_ptr, &trans, &num_trans, NULL))
        num_trans = 0;

    rows = png_get_rows(png_ptr, info_ptr);

    fout = fopen(argv[2], "w+");
    if (!fout)
        abort_("File %s could not be opened for writing", argv[2]);

    write_palette(fout);
    fprintf(fout, "\n");
    write_data(fout);

    png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);

    fclose(fin);
    fclose(fout);

    return 0;
}
