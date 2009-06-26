# what flags you want to pass to the C compiler & linker
# AM_CFLAGS = --pedantic -Wall  -O2 `taglib-config --cflags` `pkg-config --cflags gtk+-2.0 gstreamer-0.10`
# AM_LDFLAGS = `taglib-config --libs` `pkg-config --libs gtk+-2.0 gstreamer-0.10` -ltag_c
AM_CFLAGS = 	-Wall -O2 \
  @GTK_CFLAGS@ \
  @GSTREAMER_CFLAGS@ \
  @TAGLIB_CFLAGS@
AM_LDFLAGS =	@GTK_LIBS@ \
  @GSTREAMER_LIBS@ \
  @TAGLIB_LIBS@

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = tabu
tabu_SOURCES = tabu.c tbw.h tabu_player.c tbw_controls.c tbw_window.c

desktopdir = $(datadir)/applications
desktop_DATA = tabu.desktop

pixmap_icondir = $(datadir)/pixmaps
pixmap_icon_DATA = tabu.png

EXTRA_DIST = $(desktop_DATA) $(pixmap_icon_DATA)
