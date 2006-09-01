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

/* Convert GIMP raw indexed image to Dreamcast icon. Some versions of GIMP
 * only export half the image in RAW format, so double the vertical size
 * of the image before exporting.
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define PAL_ENTRIES 16
#define PAL_SIZE (PAL_ENTRIES*3)
unsigned char palette[PAL_SIZE];

#define IMAGE_WIDTH 32
#define DATA_SIZE (IMAGE_WIDTH*IMAGE_WIDTH*2)
unsigned char pixels[DATA_SIZE];

static void abort_(const char * s, ...)
{
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    fprintf(stderr, "\n");
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

    fprintf(f, "char palette[] = {\n");

    for (i = 0; i < PAL_ENTRIES; i++)
    {
        int argb = to_argb4444(palette[i * 3], palette[i * 3 + 1], palette[i * 3 + 2], 255);
        fprintf(f, "    0x%02x, 0x%02x", argb & 0xff, argb >> 8);
        if (i != PAL_ENTRIES - 1)
            fprintf(f, ", /* %2i */\n", i);
        else
            fprintf(f, "  /* %2i */\n};\n", i);
    }
}

int combine_pixel(int nibble1, int nibble2)
{
    return (((nibble1 & 0x0f) << 4) | (nibble2 & 0x0f));
}

void write_data(FILE *f)
{
    int i;

    fprintf(f, "char pixels[] = {\n");

    for (i = 0; i < IMAGE_WIDTH * 2; i++)
    {
        int j;

        fprintf(f, "    ");

        for (j = 0; j < IMAGE_WIDTH / 4; j++)
        {
            int index = i * IMAGE_WIDTH + j * 2 * 2;
            fprintf(f, "0x%02x", combine_pixel(pixels[index], pixels[index + 2]));
            if (j != IMAGE_WIDTH / 4 - 1)
                fprintf(f, ", ");
        }

        if (i != IMAGE_WIDTH * 2 - 1)
            fprintf(f, ",\n");
        else
            fprintf(f, "\n};\n");
    }
}

int main(int argc, char **argv)
{
    FILE *f;
    int i;

    if (argc < 4)
    {
        printf("Usage: dcicon <icon.raw> <icon.raw.pal> <icon.c>\n");
        exit(0);
    }

    f = fopen(argv[1], "rb");
    if (!f)
        abort_("File %s could not be opened for reading", argv[1]);

    if (fread(pixels, DATA_SIZE, 1, f) < 1)
        abort_("Error reading %s", argv[1]);

    fclose(f);

    f = fopen(argv[2], "rb");
    if (!f)
        abort_("File %s could not be opened for reading", argv[2]);

    if (fread(palette, PAL_SIZE, 1, f) < 1)
        abort_("Error reading %s", argv[2]);

    fclose(f);

    f = fopen(argv[3], "w+");
    if (!f)
        abort_("File %s could not be opened for writing", argv[3]);

    write_palette(f);
    fprintf(f, "\n");
    write_data(f);

    fclose(f);

    return 0;
}
