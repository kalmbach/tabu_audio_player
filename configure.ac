#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([tabu_audio_player], [1.0], [kalmbach@gmail.com])
AM_INIT_AUTOMAKE(tabu_audio_player, 1.0)
AC_CONFIG_SRCDIR([tbw_window.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
dnl pkg-config
PKG_PROG_PKG_CONFIG

dnl GLIB
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.8.0 gobject-2.0 gthread-2.0 gmodule-2.0])

dnl GTK
PKG_CHECK_MODULES([GTK], [gtk+-2.0 >= 2.10.0])

dnl TAGLIB
PKG_CHECK_MODULES([TAGLIB], [taglib >= 1.5])
# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_OUTPUT
