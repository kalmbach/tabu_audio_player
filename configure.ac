#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([tabu_audio_player], 1.4.1, [kalmbach@gmail.com], tabu_audio_player)
AM_INIT_AUTOMAKE(tabu_audio_player, 1.4.1)
AC_CONFIG_SRCDIR([tbw_window.c])
AC_LANG(C)

dnl TABU defines
TABU_DEFINES='-DDATA_DIR="\"$(datadir)\"" -DPKGDATA_DIR="\"$(pkgdatadir)\""'
AC_SUBST([TABU_DEFINES])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])

# Checks for programs.
AC_PROG_CC

AC_ARG_ENABLE(debug,
    AC_HELP_STRING([  --enable-debug], [Enable debugging support]), [debug="yes"])


# Checks for libraries.
dnl pkg-config
PKG_PROG_PKG_CONFIG

dnl Check if debug support is enabled or not
AC_MSG_CHECKING(for debug support)
if test "x$debug" = "xyes" ; then
    AC_DEFINE(GFPM_DEBUG, 1, [Debug support: enabled])
    CFLAGS="${CFLAGS} -ggdb3 -Wall -DTABU_DEBUG"
    debug=yes
    AC_MSG_RESULT(yes)
else
    AC_DEFINE(GFPM_DEBUG, 0, [Debug support: disabled])
    CFLAGS="${CFLAGS} -Wall"
    debug=no
    AC_MSG_RESULT(no)
fi

AC_SUBST([TABU_DEBUG])
ALL_LINGUAS=""
GETTEXT_PACKAGE=tabu
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [needed for bindtextdomain() and textdomain()])
AM_GLIB_GNU_GETTEXT
AM_GLIB_DEFINE_LOCALEDIR(LOCALEDIR)


dnl GLIB
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.8.0 gobject-2.0 gthread-2.0 gmodule-2.0])

dnl GTK
PKG_CHECK_MODULES([GTK], [gtk+-2.0 >= 2.10.0])

dnl GSTREAMER
PKG_CHECK_MODULES([GSTREAMER], [gstreamer-0.10 >= 0.10])

dnl TAGLIB
# PKG_CHECK_MODULES([TAGLIB], [taglib >= 1.5])
PKG_CHECK_MODULES([TAGLIB], [taglib_c >= 1.5])

AC_OUTPUT

echo "
Tabu $VERSION configuration summary:

        prefix                   : ${prefix}
        source code location     : ${srcdir}
        compiler                 : ${CC}
        compiler flags           : ${CFLAGS}
	debug support            : ${debug}
"

