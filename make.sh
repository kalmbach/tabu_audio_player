#!/bin/sh

gcc tabu.c tabu_player.c tbw_window.c tbw_controls.c tbw.h owl-audio-player.h -Wall -o tabu `taglib-config --cflags --libs` `pkg-config --cflags --libs gtk+-2.0 gstreamer-0.10` -ltag_c

