# what flags you want to pass to the C compiler & linker
AM_CFLAGS = -Wall  -O2 `taglib-config --cflags` `pkg-config --cflags gtk+-2.0 gstreamer-0.10`
AM_LDFLAGS = `taglib-config --libs` `pkg-config --libs gtk+-2.0 gstreamer-0.10` -ltag_c

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = tabu
tabu_SOURCES = tabu.c tbw.h tabu_player.c tbw_controls.c tbw_window.c
