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

# Author Matt Flax <flatmax@flatmax.org>

AUTOMAKE_OPTIONS = foreign subdir-objects
SUBDIRS = src api include

if !HAVE_EMSCRIPTEN
SUBDIRS += applications

if HAVE_TESTS
SUBDIRS += test
endif

endif

ACLOCAL_AMFLAGS = -I m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = gtkIOStream.pc gtkIOStreamORB.pc

EXTRA_DIST = README changeLog.txt gpl.txt VS2012 debian
