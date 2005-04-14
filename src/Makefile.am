bin_PROGRAMS = dcmstrip ttf2png
dcmstrip_SOURCES = dcmstrip.cpp
dcmstrip_LDADD = -ltristripper
AM_CFLAGS = $(CFLAGS) @SDL_CFLAGS@
ttf2png_SOURCES = ttf2png.c
ttf2png_LDADD = @SDL_TTF_LIBS@ @SDL_LIBS@ @PNG_LIBS@
