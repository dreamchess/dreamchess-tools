#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.56)
AC_INIT([dreamchess-tools],[0.1.0-SVN])
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/dcmstrip.cpp])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX

AC_DEFUN([AC_CHECK_SDL_HEADER],
	OLD_CFLAGS="${CFLAGS}"
	CFLAGS="${CFLAGS} ${SDL_CFLAGS}"
	AC_MSG_CHECKING([for $1])
	AC_COMPILE_IFELSE(
		[AC_LANG_SOURCE([[#include "$1"]])],
		[AC_MSG_RESULT([yes]) ; $2],
		[AC_MSG_RESULT([no]) ; $3]
		)
	CFLAGS="${OLD_CFLAGS}"
	)

AC_DEFUN([AC_NEEDS],
	OLD_LIBS="${LIBS}"
	LIBS="${LIBS} -l$1 $6"
	[AC_CACHE_CHECK([whether lib$1 needs -l$2], ac_$1_needs_$2,
		AC_TRY_LINK_FUNC($3,
			[ac_$1_needs_$2="no" ; $5],
			[ac_$1_needs_$2="yes" ; $4]
			)
		)]
	LIBS="${OLD_LIBS}"
	)

AM_PATH_SDL([1.2.0], [], [AC_MSG_ERROR([Cannot find SDL.])])

AC_CHECK_SDL_HEADER([SDL_ttf.h], [], [AC_MSG_ERROR([Cannot find
	SDL_ttf header file])]
	)

AC_CHECK_HEADER([png.h], [], [AC_MSG_ERROR([Cannot find png header file.])])

AC_CHECK_LIB(m, sqrt, [have_m="yes" M_LIBS="-lm"], [have_m="no"])

AC_CHECK_LIB(z, compress, [have_z="yes" Z_LIBS="-lz"],
	[have_z="no"]
	)

AC_CHECK_LIB(freetype, FT_Load_Glyph, [have_freetype="yes"
	FREETYPE_LIBS="-lfreetype"], [have_freetype="no"]
	)

AC_CHECK_LIB([SDL_ttf], [TTF_OpenFont],
	[SDL_TTF_LIBS="-lSDL_ttf"],
	[AC_MSG_ERROR([Cannot find SDL_ttf library.])],
	[${FREETYPE_LIBS} ${M_LIBS} ${SDL_LIBS}]
	)

if test x"$have_freetype" = xyes; then
	AC_NEEDS(SDL_ttf, freetype, TTF_OpenFont,
		[SDL_TTF_LIBS="${SDL_TTF_LIBS} ${FREETYPE_LIBS}"],
		[], [${M_LIBS} ${SDL_LIBS}]
	)
fi

if test x"$have_m" = xyes; then
	AC_NEEDS(SDL_ttf, m, TTF_OpenFont,
		[SDL_TTF_LIBS="${SDL_TTF_LIBS} ${M_LIBS}"],
		[], [${FREETYPE_LIBS} ${SDL_LIBS}]
	)
fi

AC_CHECK_LIB(png, png_create_read_struct, [PNG_LIBS="-lpng"],
	[AC_MSG_ERROR([Cannot find png library.])],
	[${Z_LIBS} ${M_LIBS}]
	)

if test x"$have_m" = xyes; then
	AC_NEEDS(png, m, png_create_read_struct,
		[PNG_LIBS="${PNG_LIBS} -lm"], [], [${Z_LIBS}]
		)
fi

if test x"$have_z" = xyes; then
	AC_NEEDS(png, z, png_create_read_struct,
		[PNG_LIBS="${PNG_LIBS} -lz"], [], [${M_LIBS}]
		)
fi

AC_SUBST(PNG_LIBS)
AC_SUBST(SDL_TTF_LIBS)

AC_LANG(C++)
AC_CHECK_HEADER(tristripper/tri_stripper.h, [],
[AC_MSG_ERROR([Can't find tristripper header file.])])
AC_CHECK_LIB(tristripper, main, :,
	[AC_MSG_ERROR([Can't find tristripper library.])])

AC_CONFIG_FILES([Makefile
		src/Makefile])

AC_OUTPUT
