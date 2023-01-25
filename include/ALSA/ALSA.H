/* Copyright 2000-2021 Matt Flax <flatmax@flatmax.org>
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

	#define PCM_NOT_OPEN_CHECK_STR_NO_PRINT(pcm, dbgClass) {if (!pcm) return dbgClass.errors[ALSA_PCM_NOT_OPEN_ERROR].c_str();}
	#define PCM_NOT_OPEN_CHECK_NO_PRINT(pcm, type) {if (!pcm) return (type)ALSADebug().evaluateError(ALSA_PCM_NOT_OPEN_ERROR);}
	#define PCM_NOT_OPEN_CHECK_TYPED(pcm, type) {printf("\t\tfunc: %s\n",__func__); PCM_NOT_OPEN_CHECK_NO_PRINT(pcm, type)}
	#define PCM_NOT_OPEN_CHECK(pcm) PCM_NOT_OPEN_CHECK_TYPED(pcm, int)
	#define PCM_NOT_OPEN_CHECK_STRING(pcm) {printf("\t\tfunc: %s\n",__func__); if (!pcm) return "PCM not open error";}

	#define CONTROL_NOT_OPEN_CHECK_NO_PRINT(ctl, type) {if (!ctl) return (type)ALSADebug().evaluateError(ALSA_CONTROL_NOT_OPEN_ERROR);}
	#define CONTROL_NOT_OPEN_CHECK_TYPED(ctl, type) {printf("\t\tfunc: %s\n",__func__); CONTROL_NOT_OPEN_CHECK_NO_PRINT(ctl, type)}
	#define CONTROL_NOT_OPEN_CHECK(ctl) CONTROL_NOT_OPEN_CHECK_TYPED(ctl, int)

	#define INFO_NOT_OPEN_CHECK_NO_PRINT(info, type) {if (!info) return (type)ALSADebug().evaluateError(ALSA_INFO_NOT_OPEN_ERROR);}
	#define INFO_NOT_OPEN_CHECK_TYPED(info, type) {printf("\t\tfunc: %s\n",__func__); INFO_NOT_OPEN_CHECK_NO_PRINT(info, type)}
	#define INFO_NOT_OPEN_CHECK_STR(info, dbgClass) {printf("\t\tfunc: %s\n",__func__); if (!info) return dbgClass.errors[ALSA_INFO_NOT_OPEN_ERROR].c_str();}
	#define INFO_NOT_OPEN_CHECK(info) INFO_NOT_OPEN_CHECK_TYPED(info, int)

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
#include <ALSA/MixerEvents.H>
#include <ALSA/Control.H>
#include <ALSA/Info.H>

namespace ALSA {
	/** Set the current thread's priority
	\param priority <0 implies maximum priority, otherwise must be between sched_get_priority_max and sched_get_priority_min
	\return 0 on success, error code otherwise
	*/
	static int changeThreadPriority(int priority){
		int ret;
		pthread_t thisThread = pthread_self(); // get the current thread
		struct sched_param origParams, params;
		int origPolicy, policy = SCHED_FIFO, newPolicy=0;

		if ((ret = pthread_getschedparam(thisThread, &origPolicy, &origParams))!=0)
			return ALSA::ALSADebug().evaluateError(ret, "when trying to pthread_getschedparam\n");
		printf("ALSA::Stream::changeThreadPriority : Current thread policy %d and priority %d\n", origPolicy, origParams.sched_priority);

		if (priority<0) //maximum priority
			params.sched_priority = sched_get_priority_max(policy);
		else
			params.sched_priority = priority;

		if (params.sched_priority>sched_get_priority_max(policy))
			return ALSA::ALSADebug().evaluateError(ALSA_SCHED_PRIORITY_ERROR, "requested priority is too high\n");
		if (params.sched_priority<sched_get_priority_min(policy))
			return ALSA::ALSADebug().evaluateError(ALSA_SCHED_PRIORITY_ERROR, "requested priority is too low\n");

		if ((ret = pthread_setschedparam(thisThread, policy, &params))!=0)
			return ALSA::ALSADebug().evaluateError(ret, "when trying to pthread_setschedparam - are you su or do you have permission to set this priority?\n");
		if ((ret = pthread_getschedparam(thisThread, &newPolicy, &params))!=0)
			return ALSA::ALSADebug().evaluateError(ret, "when trying to pthread_getschedparam\n");
		if(policy != newPolicy)
			return ALSA::ALSADebug().evaluateError(ALSA_SCHED_POLICY_ERROR, "requested scheduler policy is not correctly set\n");
		printf("ALSA::Stream::changeThreadPriority : New thread priority changed to %d\n", params.sched_priority);

		return 0;
	}

	/** Set the current thread's priority
	\param priority <0 implies maximum priority, otherwise must be between sched_get_priority_max and sched_get_priority_min
	\return 0 on success, error code otherwise
	*/
	static int getThreadPriority(void){
		int ret;
		pthread_t thisThread = pthread_self(); // get the current thread
		struct sched_param origParams, params;
		int origPolicy;

		if ((ret = pthread_getschedparam(thisThread, &origPolicy, &origParams))!=0)
			return ALSA::ALSADebug().evaluateError(ret, "when trying to pthread_getschedparam\n");
		printf("ALSA::Stream::changeThreadPriority : Current thread policy %d and priority %d\n", origPolicy, origParams.sched_priority);
		return origParams.sched_priority;
	}
}
#endif // ALSA_H
