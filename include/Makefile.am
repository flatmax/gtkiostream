#   Copyright 2000-2014 Matt Flax <flatmax@flatmax.org>
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

otherincludedir = $(includedir)/gtkIOStream

otherinclude_HEADERS = Alignment.H Container.H GtkUtils.H OptionParser.H Selection.H Box.H Debug.H JackClient.H ORB.H Separator.H \
                       Buttons.H DrawingArea.H Labels.H Pango.H Sox.H CairoArrow.H EventBox.H Pixmap.H Table.H ColourLineSpec.H FileGtk.H MessageDialog.H Plot.H \
                       TextView.H colourWheel.H Frame.H ProgressBar.H Thread.H ComboBoxText.H gtkDialog.H NeuralNetwork.H Scales.H Widget.H \
                       commonTimeCodeX.H gtkInterface.H Octave.H Scrolling.H WSOLA.H WSOLAJack.H Surface.H SelectionArea.H CairoBox.H DirectoryScanner.H BlockBuffer.H \
                       DragNDrop.H CairoArc.H CairoCircle.H JackBase.H JackPortMonitor.H BitStream.H FileDialog.H Window.H

if CYGWIN
otherinclude_HEADERS += TimeTools.H
else
otherinclude_HEADERS += Time.H
endif

if HAVE_ZEROC_ICE
#nodist_otherinclude_HEADERS = ../src/machineGeneratedGtkiostream/ORBICE.H
endif

oldincludedir = $(includedir)/gtkIOStream
nobase_oldinclude_HEADERS = mffm/BST.H mffm/HeapTreeType.H mffm/HeapTree.H mffm/LinkList.H fft/ComplexFFTData.H fft/ComplexFFT.H fft/FFTCommon.H fft/Real2DFFTData.H \
                            fft/Real2DFFT.H fft/RealFFTData.H fft/RealFFT.H AudioMask/AudioMasker.H AudioMask/AudioMask.H AudioMask/depukfb.H AudioMask/fastDepukfb.H \
                            AudioMask/MooreSpread.H AudioMask/AudioMaskCommon.H DSP/Decomposition.H  DSP/OverlapAdd.H \
                            IIO/IIO.H IIO/IIODevice.H IIO/IIOChannel.H IIO/IIOThreaded.H IIO/IIOThreadedQ.H IIO/IIOMMap.H posixForMicrosoft/dirent.h \
                            ALSA/ALSA.H ALSA/ALSAExternalPlugin.H ALSA/FullDuplex.H ALSA/PCM.H ALSA/Software.H \
														ALSA/Capture.H ALSA/Hardware.H ALSA/Playback.H ALSA/Stream.H

nobase_oldinclude_HEADERS += xpm/play.xpm

EXTRA_DIST = Examples.H
