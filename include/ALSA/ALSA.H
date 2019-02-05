/* Copyright 2000-2018 Matt Flax <flatmax@flatmax.org>
   This file is part of GTK+ IOStream class set

   GTK+ IOStream is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   GTK+ IOStream is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You have received a copy of the GNU General Public License
   along with GTK+ IOStream
*/
#ifndef ALSA_H
#define ALSA_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-attributes"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <Eigen/Dense>
#pragma GCC diagnostic pop

namespace ALSA {

	#define PCM_NOT_OPEN_CHECK_NO_PRINT(pcm, type) {if (!pcm) return (type)ALSADebug().evaluateError(ALSA_PCM_NOT_OPEN_ERROR);}
	#define PCM_NOT_OPEN_CHECK_TYPED(pcm, type) {printf("\t\tfunc: %s\n",__func__); PCM_NOT_OPEN_CHECK_NO_PRINT(pcm, type)}
	#define PCM_NOT_OPEN_CHECK(pcm) PCM_NOT_OPEN_CHECK_TYPED(pcm, int)
	#define PCM_NOT_OPEN_CHECK_STRING(pcm) {printf("\t\tfunc: %s\n",__func__); if (!pcm) return "PCM not open error";}

	#define CONTROL_NOT_OPEN_CHECK_NO_PRINT(ctl, type) {if (!ctl) return (type)ALSADebug().evaluateError(ALSA_CONTROL_NOT_OPEN_ERROR);}
	#define CONTROL_NOT_OPEN_CHECK_TYPED(ctl, type) {printf("\t\tfunc: %s\n",__func__); CONTROL_NOT_OPEN_CHECK_NO_PRINT(ctl, type)}
	#define CONTROL_NOT_OPEN_CHECK(ctl) CONTROL_NOT_OPEN_CHECK_TYPED(ctl, int)

	#define ALSA_DEFAULT_START_FS 48000
	#define ALSA_DEFAULT_FORMAT SND_PCM_FORMAT_S16_LE
	#define ALSA_DEFAULT_CHANNELS 2
}

#include <ALSA/ALSADebug.H>
#include <ALSA/PCM.H>
#include <ALSA/Hardware.H>
#include <ALSA/Software.H>
#include <ALSA/Stream.H>
#include <ALSA/Playback.H>
#include <ALSA/Capture.H>
#include <ALSA/FullDuplex.H>
#include <ALSA/Mixer.H>
#include <ALSA/Control.H>
#endif // ALSA_H
