#   Copyright 2000-2020 Matt Flax <flatmax@flatmax.org>
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

bin_PROGRAMS =

EXTRA_LIBS =
EXTRA_CFLAGS =

if HAVE_ALSA
bin_PROGRAMS += LatencyTester ALSAControlMonitor ALSACapabilities
if HAVE_SOX
bin_PROGRAMS += ALSACapture ALSACaptureSplitter ALSAFullDuplexFile
endif
endif

if HAVE_SOX
bin_PROGRAMS += WSOLA audioMasker HexToSox FilterGen
if HAVE_EMSCRIPTEN
else
if NOT_MINGW_SYSTEM
bin_PROGRAMS += IIOSox
endif
endif
EXTRA_CFLAGS += $(SOX_CFLAGS)
EXTRA_LIBS += $(SOX_LIBS)
if HAVE_JACK
bin_PROGRAMS += I2SEndianTest CrossoverTester #JackFullDuplex
endif
endif

if HAVE_JACK
bin_PROGRAMS += JackPortMonitor
endif

if HAVE_GTK
EXTRA_LIBS += $(GTK_LIBS)
EXTRA_CFLAGS += $(GTK_CFLAGS)
if HAVE_JACK
bin_PROGRAMS += JackPortMonitorGui
if HAVE_SOX
bin_PROGRAMS += WSOLAJackGTK
endif
endif
endif

if HAVE_OCTAVE
EXTRA_CFLAGS += $(MKOCTFILE_CFLAGS)
EXTRA_LIBS += $(MKOCTFILE_LIBPATH) $(MKOCTFILE_LIBS)
endif

if HAVE_OCTAVE
if HAVE_JACK
if HAVE_SOX
bin_PROGRAMS += MixerTester
endif
endif
endif

WSOLA_SOURCES = WSOLA.C
WSOLA_CPPFLAGS = -I$(top_srcdir)/include $(EIGEN_CFLAGS) $(EXTRA_CFLAGS)
WSOLA_LDADD = $(top_builddir)/src/libgtkIOStream.la $(EXTRA_LIBS)

WSOLAJackGTK_SOURCES = WSOLAJackGTK.C
WSOLAJackGTK_CPPFLAGS = -I$(top_srcdir)/include $(EIGEN_CFLAGS) $(EXTRA_CFLAGS) $(JACK_CFLAGS) $(GTK_CFLAGS)
WSOLAJackGTK_LDADD = $(top_builddir)/src/libgtkIOStream.la $(EXTRA_LIBS) $(JACK_LIBS) $(GTK_LIBS)

audioMasker_SOURCES = audioMasker.C
audioMasker_CPPFLAGS = -I$(top_srcdir)/include $(EIGEN_CFLAGS) $(FFTW3_CFLAGS) $(EXTRA_CFLAGS)
audioMasker_LDADD = $(top_builddir)/src/libAudioMask.la $(top_builddir)/src/libfft.la $(top_builddir)/src/libgtkIOStream.la $(EXTRA_LIBS) $(FFTW3_LIBS)

IIOSox_SOURCES = IIOSox.C
IIOSox_CPPFLAGS = -I$(top_srcdir)/include $(EIGEN_CFLAGS) $(EXTRA_CFLAGS)
IIOSox_LDADD = $(top_builddir)/src/libgtkIOStream.la $(EXTRA_LIBS) -lpthread

CrossoverTester_SOURCES = CrossoverTester.C
CrossoverTester_CPPFLAGS = -I$(top_srcdir)/include $(EIGEN_CFLAGS) $(EXTRA_CFLAGS)
CrossoverTester_LDADD = $(top_builddir)/src/libdsp.la  $(top_builddir)/src/libAudioMask.la $(top_builddir)/src/libfft.la $(top_builddir)/src/libgtkIOStream.la  $(JACK_LIBS) $(FFTW3_LIBS) $(EXTRA_LIBS) -lpthread

#JackFullDuplex_SOURCES = JackFullDuplex.C
#JackFullDuplex_CPPFLAGS = -I$(top_srcdir)/include $(EIGEN_CFLAGS) $(EXTRA_CFLAGS)
#JackFullDuplex_LDADD = $(top_builddir)/src/libdsp.la  $(top_builddir)/src/libAudioMask.la $(top_builddir)/src/libfft.la $(top_builddir)/src/libgtkIOStream.la  $(JACK_LIBS) $(FFTW3_LIBS) $(EXTRA_LIBS) -lpthread

I2SEndianTest_SOURCES = I2SEndianTest.C
I2SEndianTest_CPPFLAGS = -I$(top_srcdir)/include $(JACK_CFLAGS) $(EXTRA_CFLAGS)
I2SEndianTest_LDADD =  $(JACK_LIBS)

JackPortMonitor_SOURCES = JackPortMonitor.C
JackPortMonitor_CPPFLAGS = -I$(top_srcdir)/include $(EIGEN_CFLAGS) $(EXTRA_CFLAGS) $(JACK_CFLAGS) $(GTK_CFLAGS)
JackPortMonitor_LDADD = $(top_builddir)/src/libgtkIOStream.la $(EXTRA_LIBS) $(JACK_LIBS) $(GTK_LIBS) -lpthread

JackPortMonitorGui_SOURCES = JackPortMonitorGui.C
JackPortMonitorGui_CPPFLAGS = -I$(top_srcdir)/include $(EIGEN_CFLAGS) $(EXTRA_CFLAGS) $(JACK_CFLAGS) $(GTK_CFLAGS)
JackPortMonitorGui_LDADD = $(top_builddir)/src/libgtkIOStream.la $(EXTRA_LIBS) $(JACK_LIBS) $(GTK_LIBS)  -lpthread

MixerTester_SOURCES = MixerTester.C
MixerTester_CPPFLAGS = -I$(top_srcdir)/include $(EIGEN_CFLAGS) $(EXTRA_CFLAGS)
MixerTester_LDADD = $(top_builddir)/src/libdsp.la  $(top_builddir)/src/libAudioMask.la $(top_builddir)/src/libfft.la $(top_builddir)/src/libgtkIOStream.la  $(JACK_LIBS) $(FFTW3_LIBS) $(EXTRA_LIBS) -lpthread

HexToSox_SOURCES = HexToSox.C
HexToSox_CPPFLAGS = -I$(top_srcdir)/include $(EIGEN_CFLAGS) $(EXTRA_CFLAGS)
HexToSox_LDADD = $(top_builddir)/src/libgtkIOStream.la $(EXTRA_LIBS)

FilterGen_SOURCES = FilterGen.C
FilterGen_CPPFLAGS = -I$(top_srcdir)/include $(EIGEN_CFLAGS) $(EXTRA_CFLAGS)
FilterGen_LDADD = $(top_builddir)/src/libgtkIOStream.la $(EXTRA_LIBS)

LatencyTester_SOURCES = LatencyTester.C
LatencyTester_CPPFLAGS = -I$(top_srcdir)/include $(EIGEN_CFLAGS) $(EXTRA_CFLAGS)
LatencyTester_LDADD = $(top_builddir)/src/libdsp.la $(top_builddir)/src/libgtkIOStream.la $(FFTW3_LIBS) $(EXTRA_LIBS) -lasound

ALSAControlMonitor_SOURCES = ALSAControlMonitor.C
ALSAControlMonitor_CPPFLAGS = -I$(top_srcdir)/include $(EIGEN_CFLAGS) $(EXTRA_CFLAGS)
ALSAControlMonitor_LDADD = $(top_builddir)/src/libdsp.la $(top_builddir)/src/libgtkIOStream.la $(EXTRA_LIBS) -lasound  -lpthread

ALSACapabilities_SOURCES = ALSACapabilities.C
ALSACapabilities_CPPFLAGS = -I$(top_srcdir)/include $(EIGEN_CFLAGS) $(EXTRA_CFLAGS)
ALSACapabilities_LDADD = $(top_builddir)/src/libdsp.la $(top_builddir)/src/libgtkIOStream.la $(EXTRA_LIBS) -lasound  -lpthread

ALSACapture_SOURCES = ALSACapture.C
ALSACapture_CPPFLAGS = -I$(top_srcdir)/include $(EIGEN_CFLAGS) $(EXTRA_CFLAGS)
ALSACapture_LDADD = $(top_builddir)/src/libdsp.la $(top_builddir)/src/libgtkIOStream.la $(EXTRA_LIBS) -lasound  -lpthread

ALSACaptureSplitter_SOURCES = ALSACaptureSplitter.C
ALSACaptureSplitter_CPPFLAGS = -I$(top_srcdir)/include $(EIGEN_CFLAGS) $(EXTRA_CFLAGS)
ALSACaptureSplitter_LDADD = $(top_builddir)/src/libdsp.la $(top_builddir)/src/libgtkIOStream.la $(EXTRA_LIBS) -lasound  -lpthread

ALSAFullDuplexFile_SOURCES = ALSAFullDuplexFile.C
ALSAFullDuplexFile_CPPFLAGS = -I$(top_srcdir)/include $(EIGEN_CFLAGS) $(EXTRA_CFLAGS)
ALSAFullDuplexFile_LDADD = $(top_builddir)/src/libdsp.la $(top_builddir)/src/libgtkIOStream.la $(EXTRA_LIBS) -lasound  -lpthread

#AM_CPPFLAGS             = -I$(top_srcdir)/include $(GTKDATABOX_CFLAGS)
#LDADD                   = $(GTKDATABOX_LIBS)
