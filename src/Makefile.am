#   Copyright 2000-2018 Matt Flax <flatmax@flatmax.org>
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

#SUBDIRS = deBoor
AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES = libgtkIOStream.la libAudioMask.la libfft.la
if HAVE_ZEROC_ICE
#MGGIS=machineGeneratedGtkiostream
#${MGGIS}/%.C : %.ice
#	mkdir ${MGGIS}
#	$(SLICE2CPP) ${ICE_FLAGS} --source-ext C --header-ext H  --output-dir ${MGGIS} $<

#clean-local:
#	-rm -rf ${MGGIS}

#lib_LTLIBRARIES += libORB.la

#BUILT_SOURCES = ${MGGIS}/ORBICE.C
#CLEANFILES = ${MGGIS}/ORBICE.C
#nodist_libORB_la_SOURCES = ${MGGIS}/ORBICE.C
#libORB_la_CPPFLAGS = -I$(top_srcdir)/include -I${MGGIS} -I../../src/${MGGIS} -fpermissive
#libORB_la_LDFLAGS = -version-info $(LT_CURRENT) -release $(LT_RELEASE)
#EXTRA_DIST = ORBICE.ice
endif

libgtkIOStream_la_SOURCES = WSOLA.C BitStream.C #IIO/IIO.C IIO/IIODevice.C

libgtkIOStream_la_CPPFLAGS = -I$(top_srcdir)/include $(EIGEN_CFLAGS) $(GTKDATABOX_CFLAGS) $(FFTW3_CFLAGS)
libgtkIOStream_la_LDFLAGS =  -rdynamic -version-info $(LT_CURRENT) $(GTKDATABOX_LIBS) -release $(LT_RELEASE)

lib_LTLIBRARIES += libdsp.la
libdsp_la_SOURCES = DSP/IIR.C DSP/IIRCascade.C DSP/FIR.C DSP/ImpulseBandLimited.C
libdsp_la_CPPFLAGS = -I$(top_srcdir)/include $(FFTW3_CFLAGS) $(EIGEN_CFLAGS) -DMFILE_PATH1=\"mFiles\" -DMFILE_PATH2=\"$(DESTDIR)$(docdir)/mFiles\"
libdsp_la_LDFLAGS =  -version-info $(LT_CURRENT) $(FFTW3_LIBS) -release $(LT_RELEASE)

if HAVE_SOX
libgtkIOStream_la_SOURCES += Sox.C
libgtkIOStream_la_CPPFLAGS += $(SOX_CFLAGS)
libgtkIOStream_la_LDFLAGS += $(SOX_LIBS)
endif

if HAVE_OCTAVE
#libdsp_la_SOURCES += Decomposition.C
libgtkIOStream_la_SOURCES += Octave.C
libgtkIOStream_la_CPPFLAGS += $(MKOCTFILE_CFLAGS)
libgtkIOStream_la_LDFLAGS += $(MKOCTFILE_LIBPATH) $(MKOCTFILE_LIBS)
endif

if HAVE_OPENCV
libgtkIOStream_la_CPPFLAGS += $(OPENCV_CFLAGS) -DHAVE_OPENCV
libgtkIOStream_la_LDFLAGS += $(OPENCV_LIBS)
endif

if HAVE_JACK
if HAVE_GTK
libgtkIOStream_la_SOURCES += JackPortMonitorGui.C
libgtkIOStream_la_CPPFLAGS += $(GTK_CFLAGS)
libgtkIOStream_la_LDFLAGS += $(GTK_LIBS)
endif
libgtkIOStream_la_SOURCES += JackPortMonitor.C
libgtkIOStream_la_CPPFLAGS += $(JACK_CFLAGS)
libgtkIOStream_la_LDFLAGS += $(JACK_LIBS)

libdsp_la_SOURCES += DSP/CrossoverAudio.C DSP/MixerTestAudio.C
endif


libfft_la_SOURCES = ComplexFFTData.C Real2DFFTData.C RealFFTData.C RealFFT.C
libfft_la_CPPFLAGS = -I$(top_srcdir)/include $(FFTW3_CFLAGS)
libfft_la_LDFLAGS =  -version-info $(LT_CURRENT)  $(FFTW3_LIBS) -release $(LT_RELEASE)

libAudioMask_la_SOURCES = AudioMask/AudioMask.C AudioMask/AudioMasker.C AudioMask/MooreSpread.C
libAudioMask_la_CPPFLAGS = -I$(top_srcdir)/include $(FFTW3_CFLAGS) $(EIGEN_CFLAGS)
libAudioMask_la_LDFLAGS =  -version-info $(LT_CURRENT) $(FFTW3_LIBS) -release $(LT_RELEASE)

if HAVE_EMSCRIPTEN
all-local: libgtkIOStream.la libfft.la libdsp.la
	@emcc --bind -s ALLOW_MEMORY_GROWTH=1 --llvm-lto 1 -s WASM=0 --memory-init-file 0 $(AM_CXXFLAGS) .libs/libfft.so $(FFTW3_LIBS) -o .libs/libfft.js
	mv .libs/libfft.js .libs/libfftNode.js
	@emcc --bind --llvm-lto 1 --memory-init-file 0 -s "MODULARIZE=1" -s EXPORT_NAME="'libfft'" -s "BINARYEN_METHOD='native-wasm'" -s "WASM=1" -s ALLOW_MEMORY_GROWTH=1 $(AM_CXXFLAGS) .libs/libfft.so $(FFTW3_LIBS) -o .libs/libfft.js

	@emcc --bind -s ALLOW_MEMORY_GROWTH=1 --llvm-lto 1 -s WASM=0 --memory-init-file 0 $(AM_CXXFLAGS) .libs/libgtkIOStream.so $(SOX_LIBS) -o .libs/libgtkIOStream.js
	mv .libs/libgtkIOStream.js .libs/libgtkIOStreamNode.js
	@emcc --bind --llvm-lto 1 --memory-init-file 0 --pre-js ../webcomponents/pre.js -s MODULARIZE_INSTANCE=1 -s "MODULARIZE=1" -s EXPORT_NAME="'libgtkIOStream'" -s SINGLE_FILE=1 -s "BINARYEN_METHOD='native-wasm'" -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 $(AM_CXXFLAGS) .libs/libgtkIOStream.so $(SOX_LIBS) -o .libs/libgtkIOStream.js
	cp .libs/libgtkIOStreamNode.js .libs/libgtkIOStream.js ../webcomponents/gtkiostream
	cp ../test/testVectors/11.Neutral.44k.wav ../js/SoxTest.js ../webcomponents/gtkiostream/test
install-exec-hook:
	cp .libs/libfft.js .libs/libfft.wasm .libs/libgtkIOStream.js $(DESTDIR)$(libdir) #.libs/libfft.node
clean-local:
	@rm -f .libs/libfft.node .libs/libfft.js .libs/libfft.wasm .libs/libgtkIOStream.js
endif
