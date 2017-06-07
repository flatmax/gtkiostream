#    copyright Copyright 2013 Matt R. Flax <flatmax@flatmax.org> All Rights Reserved.
#
#    This code has a restricted license, you may not use this code without the permission of Matt Flax.
#
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
