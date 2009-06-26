#!/bin/sh -e

cd `dirname $0`

if [ "$1" == "--dist" ]; then
	ver=`grep AC_INIT configure.ac|sed 's/.*, \([0-9\.]*\), .*/\1/'`
	git archive --format=tar --prefix=tabu_audio_player-$ver/ HEAD | tar xf -
	git log --no-merges |git name-rev --tags --stdin > tabu_audio_player-$ver/ChangeLog
	cd tabu_audio_player-$ver
	./autogen.sh --git
	cd ..
	tar czf tabu_audio_player-$ver.tar.gz tabu_audio_player-$ver
	rm -rf tabu_audio_player-$ver
	exit 0
fi

cat /usr/share/aclocal/libtool.m4 >> aclocal.m4

intltoolize -c -f --automake
libtoolize -f -c
aclocal --force
autoheader -f
autoconf -f
cp -f /usr/share/automake/mkinstalldirs ./
cp -f /usr/share/gettext/config.rpath ./
automake -a -c --gnu --foreign

if [ "$1" == "--git" ]; then
	rm -rf autom4te.cache
fi

