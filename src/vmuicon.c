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

/* Convert GIMP raw indexed image to Dreamcast VMU icon. */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define IMAGE_WIDTH 48
#define IMAGE_HEIGHT 32
#define DATA_SIZE (IMAGE_WIDTH*IMAGE_HEIGHT)
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

int combine_pixel(char *data)
{
    int retval, i;

    retval = 0;

    for (i = 0; i < 8; i++)
        retval |= (data[i] & 1) << (7 - i);

    return retval;
}

void write_data(FILE *f)
{
    int i;

    fprintf(f, "char pixels[] = {\n");

    for (i = 0; i < IMAGE_HEIGHT; i++)
    {
        int j;

        fprintf(f, "    ");

        for (j = 0; j < IMAGE_WIDTH / 8; j++)
        {
            int index = i * IMAGE_WIDTH + j * 8;
            fprintf(f, "0x%02x", combine_pixel(pixels + index));
            if (j != IMAGE_WIDTH / 8 - 1)
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
    FILE *f;
    int i;

    if (argc < 3)
    {
        printf("Usage: vmuicon <icon.raw> <icon.c>\n");
        exit(0);
    }

    f = fopen(argv[1], "rb");
    if (!f)
        abort_("File %s could not be opened for reading", argv[1]);

    if (fread(pixels, DATA_SIZE, 1, f) < 1)
        abort_("Error reading %s", argv[1]);

    fclose(f);

    f = fopen(argv[2], "w+");
    if (!f)
        abort_("File %s could not be opened for writing", argv[3]);

    write_data(f);

    fclose(f);

    return 0;
}
