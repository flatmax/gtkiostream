#!/bin/bash
#   Copyright 2000-2021 Matt Flax <flatmax@flatmax.org>
#   This file is part of GTK+ IOStream class set
#
#   GTK+ IOStream is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   GTK+ IOStream is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You have received a copy of the GNU General Public License
#   along with GTK+ IOStream

# clean all files which are autogenerated
make distclean
mv m4/emscripten.m4 /tmp
rm -rf autom4te.cache aclocal.m4 m4/* missing config.sub COPYING config.guess configure depcomp install-sh gtkiostream_config.h.in ltmain.sh   Makefile.in src/Makefile.in test/Makefile.in INSTALL api/Makefile.in .deps  gtkiostream-1* src/AudioMask/.deps a.out* a.wasm
mv /tmp/emscripten.m4 m4
find . -name '*~' | xargs rm -f
find . -name '.deps' | xargs rm -rf
find . -name '.libs' | xargs rm -rf
find . -name 'octave-core' | xargs rm -f
find . -name 'Makefile.in' | xargs rm -f
find . -name '.dirstamp' | xargs rm -f
rm -f compile
#rm -rf config.log gtkiostream_config.h Makefile
