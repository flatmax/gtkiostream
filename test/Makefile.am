#   Copyright 2000-2017 Matt Flax <flatmax@flatmax.org>
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

#AUTOMAKE_OPTIONS = subdir-objects

EXTRA_LIBS =
EXTRA_CFLAGS =

noinst_PROGRAMS = OptionParserTest DirectoryScannerTest NeuralNetworkTest ThreadTest BlockBufferTest
noinst_PROGRAMS += BitStreamTest BitStreamTest2 BitStreamTest3 BitStreamTest4 BitStreamTest5 BitStreamTest6
#noinst_PROGRAMS += DeBoorTest

if HAVE_GTKDATABOX
noinst_PROGRAMS += PlotTest PlotTest2 NeuralNetworkFnTest
PlotTest_SOURCES = PlotTest.C
PlotTest2_SOURCES = PlotTest2.C
PlotTest3_SOURCES = PlotTest3.C

EXTRA_LIBS += $(GTKDATABOX_LIBS)
EXTRA_CFLAGS += $(GTKDATABOX_CFLAGS)
endif

if HAVE_GTK
noinst_PROGRAMS += JackPortMonitorGuiTest TimeTest
EXTRA_LIBS += $(GTK_LIBS)
EXTRA_CFLAGS += $(GTK_CFLAGS)

noinst_PROGRAMS += AlignmentTest MessageDialogTest ScaleTest TableTest TextViewTest CairoArrowTest DrawingAreaTest ButtonsTest2 ButtonsTest PangoTest PangoTest2 ComboBoxTextTest ButtonsFontTest SeparatorTest InlineTest ProgressBarTest LabelsTest LabelsTest2 CairoBoxTest SelectionAreaTest CairoCircleTest DragNDropTest DragNDropSimpleTest DragNDropNowindowTest ScrollingTest FileDialogTest FrameTest
endif

# these need to be fixed for GTK3
if HAVE_GTK3
else
if HAVE_GTK2
noinst_PROGRAMS += SelectionTest SelectionTest2 PixmapTest
SelectionTest_SOURCES = SelectionTest.C
SelectionTest2_SOURCES = SelectionTest2.C
PixmapTest_SOURCES = PixmapTest.C
endif
endif

MessageDialogTest_SOURCES = MessageDialogTest.C
ScaleTest_SOURCES = ScaleTest.C
TableTest_SOURCES = TableTest.C
TextViewTest_SOURCES = TextViewTest.C
CairoArrowTest_SOURCES = CairoArrowTest.C
DrawingAreaTest_SOURCES = DrawingAreaTest.C
ButtonsTest2_SOURCES = ButtonsTest2.C
ButtonsTest_SOURCES = ButtonsTest.C
PangoTest_SOURCES = PangoTest.C
PangoTest2_SOURCES = PangoTest2.C
ComboBoxTextTest_SOURCES = ComboBoxTextTest.C
ButtonsFontTest_SOURCES = ButtonsFontTest.C
SeparatorTest_SOURCES = SeparatorTest.C
InlineTest_SOURCES = InlineTest.C
AlignmentTest_SOURCES = AlignmentTest.C
ProgressBarTest_SOURCES = ProgressBarTest.C
LabelsTest_SOURCES = LabelsTest.C
LabelsTest2_SOURCES = LabelsTest2.C
CairoBoxTest_SOURCES = CairoBoxTest.C
SelectionAreaTest_SOURCES = SelectionAreaTest.C
CairoCircleTest_SOURCES = CairoCircleTest.C
DragNDropTest_SOURCES = DragNDropTest.C
DragNDropSimpleTest_SOURCES = DragNDropSimpleTest.C
DragNDropNowindowTest_SOURCES = DragNDropNowindowTest.C
FileDialogTest_SOURCES = FileDialogTest.C
ScrollingTest_SOURCES = ScrollingTest.C
FrameTest_SOURCES = FrameTest.C

if HAVE_OCTAVE
noinst_PROGRAMS += RealFFTExample Real2DFFTExample ComplexFFTExample OctaveTest
noinst_PROGRAMS += AudioMaskerExample

EXTRA_LIBS += $(MKOCTFILE_LIBPATH) $(MKOCTFILE_LIBS)
EXTRA_CFLAGS += $(MKOCTFILE_CFLAGS)
endif

if HAVE_SOX
if NOT_MINGW_SYSTEM
noinst_PROGRAMS += IIOMMapTest IIOTest IIOQueueTest SoxTest
EXTRA_LIBS += $(SOX_LIBS)
endif
endif

if HAVE_OCTAVE
if HAVE_SOX
noinst_PROGRAMS += OverlapAddTest #DecompositionTest
endif
endif

TimeTest_SOURCES = TimeTest.C
OptionParserTest_SOURCES = OptionParserTest.C
ThreadTest_SOURCES = ThreadTest.C

DirectoryScannerTest_SOURCES = DirectoryScannerTest.C
DirectoryScannerTest_CPPFLAGS = -I$(abs_top_srcdir)/include $(EIGEN_CFLAGS)
DirectoryScannerTest_LDADD =

NeuralNetworkFnTest_SOURCES = NeuralNetworkFnTest.C
NeuralNetworkFnTest_CPPFLAGS = -I$(abs_top_srcdir)/include $(EIGEN_CFLAGS) $(EXTRA_CFLAGS)
NeuralNetworkFnTest_LDADD =  $(EXTRA_LIBS)

INCLUDES             = -I$(abs_top_srcdir)/include $(EXTRA_CFLAGS)
if MINGW_SYSTEM
THREADLIB                = -lwinpthread
else
THREADLIB                = -lpthread
endif
LDADD =  $(THREADLIB) $(EXTRA_LIBS)

if HAVE_JACK
noinst_PROGRAMS += JackClientTest JackPortMonitorTest
#JackOverRailTest
EXTRA_LIBS += $(JACK_LIBS)
EXTRA_CFLAGS += $(JACK_CFLAGS)
endif

JackClientTest_SOURCES = JackClientTest.C
JackClientTest_CPPFLAGS = -I$(abs_top_srcdir)/include $(JACK_CFLAGS) $(EXTRA_CFLAGS)
JackClientTest_LDADD =  $(JACK_LIBS)

JackOverRailTest_SOURCES = JackOverRailTest.C
JackOverRailTest_CPPFLAGS = -I$(abs_top_srcdir)/include $(JACK_CFLAGS) $(EXTRA_CFLAGS)
JackOverRailTest_LDADD =  $(JACK_LIBS)

JackPortMonitorTest_SOURCES = JackPortMonitorTest.C
JackPortMonitorTest_CPPFLAGS = -I$(abs_top_srcdir)/include $(EXTRA_CFLAGS)
JackPortMonitorTest_LDADD =  $(top_builddir)/src/libgtkIOStream.la $(EXTRA_LIBS) $(THREADLIB)

JackPortMonitorGuiTest_SOURCES = JackPortMonitorGuiTest.C
JackPortMonitorGuiTest_CPPFLAGS = -I$(abs_top_srcdir)/include $(EXTRA_CFLAGS)
JackPortMonitorGuiTest_LDADD = $(top_builddir)/src/libgtkIOStream.la $(EXTRA_LIBS) $(THREADLIB)

NeuralNetworkTest_SOURCES = NeuralNetworkTest.C
NeuralNetworkTest_CPPFLAGS = -I$(abs_top_srcdir)/include $(EIGEN_CFLAGS) $(EXTRA_CFLAGS)
NeuralNetworkTest_LDADD =

MG=machineGenerated
${MG}/%.C : %.ice
	mkdir -p ${MG}
	$(SLICE2CPP) ${ICE_FLAGS} --source-ext C --header-ext H  --output-dir ${MG} $<

clean-local:
	-rm -rf ${MG}

EXTRA_DIST = AlignmentTest.C BSTTest.C ButtonsFontTest.C ButtonsTest2.C ButtonsTest.C CairoArrowTest.C colourWheelTest.C ComboBoxTextTest.C DrawingAreaTest.C HeapTreeSort.C InlineTest.C JackClientTest.C LabelsTest2.C LabelsTest3.C LabelsTest.C MessageDialogTest.C NeuralNetworkFnTest.C NeuralNetworkTest.C OctaveTest.C OptionParserTest.C PangoTest2.C PangoTest.C PlotTest2.C PlotTest3.C PlotTest.C ProgressBarTest.C ScaleTest.C SelectionTest2.C SelectionTest3.C SelectionTest.C SeparatorTest.C TableTest.C TextViewTest.C ThreadTest.C CairoBoxTest.C SelectionAreaTest.C RealFFTExample.C Real2DFFTExample.C ComplexFFTExample.C ComplexFFTExample.C AudioMaskerExample.C OverlapAddTest.C DirectoryScannerTest.C IIOTest.C ScrollingTest.C #DecompositionTest.C

if HAVE_ZEROC_ICE
#noinst_PROGRAMS += ORBTest
#EXTRA_DIST += ORBTestICE.ice ORBTest.C
#BUILT_SOURCES = ${MG}/ORBTestICE.C
#nodist_ORBTest_SOURCES = ${MG}/ORBTestICE.C
#ORBTest_SOURCES = ORBTest.C
#ORBTest_CPPFLAGS = -I$(abs_top_srcdir)/include -I$(abs_top_srcdir)/test/$(MG) -I$(MG) -I$(abs_top_srcdir)/src/machineGeneratedGtkiostream -I../src/machineGeneratedGtkiostream -fpermissive $(EXTRA_CFLAGS)
#ORBTest_LDADD = $(top_builddir)/src/libORB.la -lpthread $(EXTRA_LIBS)
##if ZEROC_ICE_EMBEDDED
##ORBTest_LDADD += $(top_builddir)/src/libORB.la -lIceE -lpthread $(EXTRA_LIBS)
##endif
endif

OctaveTest_SOURCES = OctaveTest.C
OctaveTest_CPPFLAGS = -I$(abs_top_srcdir)/include  $(EIGEN_CFLAGS)
OctaveTest_LDADD = $(top_builddir)/src/libgtkIOStream.la $(EXTRA_LIBS)

RealFFTExample_SOURCES = RealFFTExample.C
RealFFTExample_CPPFLAGS = -I$(abs_top_srcdir)/include  $(EIGEN_CFLAGS) $(FFTW3_CFLAGS) $(EXTRA_CFLAGS)
RealFFTExample_LDADD = $(top_builddir)/src/libgtkIOStream.la $(top_builddir)/src/libfft.la $(EXTRA_LIBS) $(FFTW3_LIBS)

Real2DFFTExample_SOURCES = Real2DFFTExample.C
Real2DFFTExample_CPPFLAGS = -I$(abs_top_srcdir)/include  $(EIGEN_CFLAGS) $(FFTW3_CFLAGS) $(EXTRA_CFLAGS)
Real2DFFTExample_LDADD = $(top_builddir)/src/libgtkIOStream.la $(top_builddir)/src/libfft.la $(EXTRA_LIBS) $(FFTW3_LIBS)

ComplexFFTExample_SOURCES = ComplexFFTExample.C
ComplexFFTExample_CPPFLAGS = -I$(abs_top_srcdir)/include  $(EIGEN_CFLAGS) $(FFTW3_CFLAGS) $(EXTRA_CFLAGS)
ComplexFFTExample_LDADD = $(top_builddir)/src/libgtkIOStream.la $(top_builddir)/src/libfft.la $(EXTRA_LIBS) $(FFTW3_LIBS)

if HAVE_OPENCV
if HAVE_OCTAVE
noinst_PROGRAMS += OctaveOpenCVTest
EXTRA_DIST += OctaveOpenCVTest.C
OctaveOpenCVTest_SOURCES = OctaveOpenCVTest.C
OctaveOpenCVTest_CPPFLAGS = -I$(abs_top_srcdir)/include  $(EIGEN_CFLAGS) $(OPENCV_CFLAGS) -DHAVE_OPENCV $(EXTRA_CFLAGS)
OctaveOpenCVTest_LDADD = $(top_builddir)/src/libgtkIOStream.la $(EXTRA_LIBS) $(OPENCV_LIBS)
endif
endif

AudioMaskerExample_SOURCES = AudioMaskerExample.C
AudioMaskerExample_CPPFLAGS = -I$(abs_top_srcdir)/include $(EIGEN_CFLAGS) $(FFTW3_CFLAGS) $(EXTRA_CFLAGS)
AudioMaskerExample_LDADD = $(top_builddir)/src/libgtkIOStream.la $(top_builddir)/src/libAudioMask.la $(top_builddir)/src/libfft.la $(FFTW3_LIBS) $(EXTRA_LIBS)

DecompositionTest_SOURCES = DecompositionTest.C
DecompositionTest_CPPFLAGS = -I$(abs_top_srcdir)/include $(EIGEN_CFLAGS) $(FFTW3_CFLAGS) $(EXTRA_CFLAGS)#-fopenmp
DecompositionTest_LDADD = $(top_builddir)/src/libdsp.la $(top_builddir)/src/libgtkIOStream.la $(top_builddir)/src/libAudioMask.la $(top_builddir)/src/libfft.la $(FFTW3_LIBS) $(EXTRA_LIBS)

OverlapAddTest_SOURCES = OverlapAddTest.C
OverlapAddTest_CPPFLAGS = -I$(abs_top_srcdir)/include $(EIGEN_CFLAGS) $(FFTW3_CFLAGS) $(EXTRA_CFLAGS)
OverlapAddTest_LDADD = $(top_builddir)/src/libgtkIOStream.la $(top_builddir)/src/libAudioMask.la $(top_builddir)/src/libdsp.la $(top_builddir)/src/libfft.la $(FFTW3_LIBS) $(EXTRA_LIBS)

BlockBufferTest_SOURCES = BlockBufferTest.C
BlockBufferTest_CPPFLAGS = -I$(abs_top_srcdir)/include $(EIGEN_CFLAGS) -fpermissive $(EXTRA_CFLAGS)
#BlockBufferTest_LDADD = $(LDADD)

SoxTest_SOURCES = SoxTest.C
SoxTest_CPPFLAGS = -I$(abs_top_srcdir)/include $(EIGEN_CFLAGS) $(EXTRA_CFLAGS)
SoxTest_LDADD = $(top_builddir)/src/libgtkIOStream.la $(LDADD)

if NOT_MINGW_SYSTEM
#these will not work on win32
IIOTest_SOURCES = IIOTest.C
IIOTest_CPPFLAGS = -I$(abs_top_srcdir)/include $(EIGEN_CFLAGS) $(EXTRA_CFLAGS)
IIOTest_LDADD = $(top_builddir)/src/libgtkIOStream.la $(LDADD)

IIOQueueTest_SOURCES = IIOQueueTest.C
IIOQueueTest_CPPFLAGS = -I$(abs_top_srcdir)/include $(EIGEN_CFLAGS) -fpermissive $(EXTRA_CFLAGS)
IIOQueueTest_LDADD = $(top_builddir)/src/libgtkIOStream.la $(LDADD) $(FFTW3_LIBS)

IIOMMapTest_SOURCES = IIOMMapTest.C
IIOMMapTest_CPPFLAGS = -I$(abs_top_srcdir)/include $(EIGEN_CFLAGS) -fpermissive $(EXTRA_CFLAGS)
IIOMMapTest_LDADD = $(top_builddir)/src/libgtkIOStream.la $(LDADD) $(FFTW3_LIBS)
endif

BitStreamTest_SOURCES = BitStreamTest.C
BitStreamTest_CPPFLAGS = -I$(abs_top_srcdir)/include $(EIGEN_CFLAGS) -fpermissive $(EXTRA_CFLAGS)
BitStreamTest_LDADD = $(top_builddir)/src/libgtkIOStream.la $(LDADD) $(FFTW3_LIBS)

BitStreamTest2_SOURCES = BitStreamTest2.C
BitStreamTest2_CPPFLAGS = -I$(abs_top_srcdir)/include $(EIGEN_CFLAGS) -fpermissive $(EXTRA_CFLAGS)
BitStreamTest2_LDADD = $(top_builddir)/src/libgtkIOStream.la $(LDADD) $(FFTW3_LIBS)

BitStreamTest3_SOURCES = BitStreamTest3.C
BitStreamTest3_CPPFLAGS = -I$(abs_top_srcdir)/include $(EIGEN_CFLAGS) -fpermissive $(EXTRA_CFLAGS)
BitStreamTest3_LDADD = $(top_builddir)/src/libgtkIOStream.la $(LDADD) $(FFTW3_LIBS)

BitStreamTest4_SOURCES = BitStreamTest4.C
BitStreamTest4_CPPFLAGS = -I$(abs_top_srcdir)/include $(EIGEN_CFLAGS) -fpermissive $(EXTRA_CFLAGS)
BitStreamTest4_LDADD = $(top_builddir)/src/libgtkIOStream.la $(LDADD) $(FFTW3_LIBS)

BitStreamTest5_SOURCES = BitStreamTest5.C
BitStreamTest5_CPPFLAGS = -I$(abs_top_srcdir)/include $(EIGEN_CFLAGS) -fpermissive $(EXTRA_CFLAGS)
BitStreamTest5_LDADD = $(top_builddir)/src/libgtkIOStream.la $(LDADD) $(FFTW3_LIBS)

BitStreamTest6_SOURCES = BitStreamTest6.C
BitStreamTest6_CPPFLAGS = -I$(abs_top_srcdir)/include $(EIGEN_CFLAGS) -fpermissive $(EXTRA_CFLAGS)
BitStreamTest6_LDADD = $(top_builddir)/src/libgtkIOStream.la $(LDADD) $(FFTW3_LIBS)

DeBoorTest_SOURCES = DeBoorTest.C
DeBoorTest_CPPFLAGS = -I$(abs_top_srcdir)/include
#$(EIGEN_CFLAGS) -fpermissive $(EXTRA_CFLAGS)
DeBoorTest_LDADD = $(top_builddir)/src/deBoor/libSpline.la $(LDADD)
# $(FFTW3_LIBS)

if HAVE_ALSA
noinst_PROGRAMS += ALSAPlaybackTest ALSACaptureTest ALSAFullDuplexTest

ALSAPlaybackTest_SOURCES = ALSAPlaybackTest.C
ALSAPlaybackTest_CPPFLAGS = -I$(abs_top_srcdir)/include $(ALSA_CFLAGS) $(EIGEN_CFLAGS)
ALSAPlaybackTest_LDADD = $(top_builddir)/src/libgtkIOStream.la $(ALSA_LIBS)  $(LDADD)

ALSACaptureTest_SOURCES = ALSACaptureTest.C
ALSACaptureTest_CPPFLAGS = -I$(abs_top_srcdir)/include $(ALSA_CFLAGS) $(EIGEN_CFLAGS)
ALSACaptureTest_LDADD = $(top_builddir)/src/libgtkIOStream.la $(ALSA_LIBS)  $(LDADD)

ALSAFullDuplexTest_SOURCES = ALSAFullDuplexTest.C
ALSAFullDuplexTest_CPPFLAGS = -I$(abs_top_srcdir)/include $(ALSA_CFLAGS) $(EIGEN_CFLAGS)
ALSAFullDuplexTest_LDADD = $(top_builddir)/src/libgtkIOStream.la $(ALSA_LIBS)  $(LDADD)

pkglib_LTLIBRARIES = libasound_module_pcm_ALSAPluginTest.la libasound_module_pcm_ALSAExternalPluginTest.la
libasound_module_pcm_ALSAPluginTest_la_SOURCES = ALSAPluginTest.C
libasound_module_pcm_ALSAPluginTest_la_CPPFLAGS = $(EIGEN_CFLAGS)
libasound_module_pcm_ALSAPluginTest_la_LDFLAGS = -module -avoid-version -export-dynamic -no-undefined
libasound_module_pcm_ALSAExternalPluginTest_la_SOURCES = ALSAExternalPluginTest.C
libasound_module_pcm_ALSAExternalPluginTest_la_CPPFLAGS = $(EIGEN_CFLAGS)
libasound_module_pcm_ALSAExternalPluginTest_la_LDFLAGS = -module -avoid-version -export-dynamic -no-undefined
endif
