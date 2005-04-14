#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.56)
AC_INIT([dreamchess-tools],[0.1.0-SVN])
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/dcmstrip.cpp])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX

AC_LANG(C++)
AC_CHECK_HEADER(tristripper/tri_stripper.h, [],
[AC_MSG_ERROR([Can't find tristripper header file.])])
AC_CHECK_LIB(tristripper, main, :,
	[AC_MSG_ERROR([Can't find tristripper library.])])

AC_CONFIG_FILES([Makefile
		src/Makefile])

AC_OUTPUT
