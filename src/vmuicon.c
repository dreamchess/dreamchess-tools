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

/* Convert 48x32 1bpp grayscale PNG image to Dreamcast VMU icon. */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <png.h>

static png_structp png_ptr;
static png_infop info_ptr;
static png_bytep *rows;

#define IMAGE_WIDTH 48
#define IMAGE_HEIGHT 32

static void abort_(const char * s, ...)
{
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    va_end(args);
    abort();
}

void write_data(FILE *f)
{
    int i;

    fprintf(f, "char pixels[%i] = {\n", IMAGE_WIDTH * IMAGE_HEIGHT / 8);

    for (i = IMAGE_HEIGHT - 1; i >= 0; i--)
    {
        int j;

        fprintf(f, "    ");

        for (j = IMAGE_WIDTH / 8 - 1; j >= 0; j--)
        {
            fprintf(f, "0x%02x", rows[i][j]);
            if (j != 0)
                fprintf(f, ", ");
        }

        if (i != 0)
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
        printf("Usage: vmuicon <icon.raw> <icon.c>\n");
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
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_INVERT_MONO | PNG_TRANSFORM_PACKSWAP, png_voidp_NULL);

    if ((png_get_image_width(png_ptr, info_ptr) != IMAGE_WIDTH) ||
        (png_get_image_height(png_ptr, info_ptr) != IMAGE_HEIGHT))
        abort_("Image is not %ix%i pixels\n", IMAGE_WIDTH, IMAGE_HEIGHT);

    if ((png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_GRAY)
        || (png_get_bit_depth(png_ptr, info_ptr) != 1))
        abort_("Image is not a grayscale image with bit depth 1\n");

    rows = png_get_rows(png_ptr, info_ptr);

    fout = fopen(argv[2], "w+");
    if (!fout)
        abort_("File %s could not be opened for writing", argv[2]);

    write_data(fout);

    png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);

    fclose(fin);
    fclose(fout);

    return 0;
}
