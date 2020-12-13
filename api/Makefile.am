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
if HAVE_DOXYGEN
dist_man_MANS = html
html: doxyfile.stamp

doxyfile.stamp:
	$(DOXYGEN) Doxyfile
	echo Timestamp > doxyfile.stamp

CLEANFILES = doxyfile.stamp #html/*

all-local: doxyfile.stamp
clean-local:
	-rm -rf html

EXTRA_DIST = analytics/header.html

install-data-local:
	$(MKDIR_P) $(DESTDIR)$(docdir)
	cp -r html $(DESTDIR)$(docdir)

uninstall-local:
	$(RM) -fr $(DESTDIR)$(docdir)/html
endif
