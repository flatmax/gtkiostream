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
#ifndef ALSADEBUG_H
#define ALSADEBUG_H

#include <iostream>
#include <Debug.H>
#include <alsa/asoundlib.h>

namespace ALSA {
	#define ALSA_PCM_NOT_OPEN_ERROR -1+ALSA_ERROR_OFFSET ///< error when pcm pointer is null
	#define ALSA_UNKNOWN_READ_ERROR -2+ALSA_ERROR_OFFSET ///< error when reading and an unknown code is retured
	#define ALSA_FULLDUPLEX_PROCESS_ERROR -3+ALSA_ERROR_OFFSET ///< error when processing the full duplex method.
	#define ALSA_FRAME_MISMATCH_ERROR -4+ALSA_ERROR_OFFSET ///< error when frame sizes are incorrect
	#define ALSA_NO_CHANNELS_ERROR -5+ALSA_ERROR_OFFSET ///< error when channel cnt is zero
	#define ALSA_YOUR_PROCESS_FN_ERROR -6+ALSA_ERROR_OFFSET ///< error when calling the user's process function
	#define ALSA_FORMAT_MISMATCH_ERROR -7+ALSA_ERROR_OFFSET ///< error when comparing bits sizes of two words
	#define ALSA_MIXER_NOT_OPEN_ERROR -8+ALSA_ERROR_OFFSET ///< error when mixer pointer is null
	#define ALSA_MIXER_QUERY_ERROR -9+ALSA_ERROR_OFFSET ///< error when attempting to query the mixer
	#define ALSA_MIXER_FIND_ELEMENT_ERROR -10+ALSA_ERROR_OFFSET ///< error when trying to find a mixer element
	#define ALSA_MIXER_WRONG_ELEMENT_ERROR -11+ALSA_ERROR_OFFSET ///< error when comparing two mixer elements.
	#define ALSA_MIXER_NO_CHANNEL_ERROR -12+ALSA_ERROR_OFFSET ///< error when we can't find the prescribed channel
	#define ALSA_MIXER_NO_PLAYBACK_VOL_ERROR -13+ALSA_ERROR_OFFSET ///< error this mixer element is not a playback element
	#define ALSA_MIXER_NO_CAPTURE_VOL_ERROR -14+ALSA_ERROR_OFFSET ///< error this mixer element is not a playback element
	#define ALSA_CONTROL_NOT_OPEN_ERROR -15+ALSA_ERROR_OFFSET ///< error when ctls pointer is null
	#define ALSA_SCHED_PRIORITY_ERROR -16+ALSA_ERROR_OFFSET ///< error when sched. priority is out of bounds
	#define ALSA_SCHED_POLICY_ERROR -17+ALSA_ERROR_OFFSET ///< error relating to the scheduler priority
	#define ALSA_MIXER_NO_ENUM_ERROR -18+ALSA_ERROR_OFFSET ///< error this mixer element is not a generic enum
	#define ALSA_INFO_NOT_OPEN_ERROR -19+ALSA_ERROR_OFFSET ///< error when info pointer is null
	#define ALSA_MIXER_NO_ELEMENT_ERROR -20+ALSA_ERROR_OFFSET ///< error the specified mixer element is null
	#define ALSA_CONFIG_NOT_OPEN_ERROR -21+ALSA_ERROR_OFFSET ///< error when the config file isn't open
	#define ALSA_UNHANDLED_TYPE -22+ALSA_ERROR_OFFSET ///< error when trying to handle an unknown type

	class ALSADebug : public Debug {
	public:
		ALSADebug(void) {
			#ifndef NDEBUG
			errors[ALSA_PCM_NOT_OPEN_ERROR]=std::string("The pcm device isn't open, open first.");
			errors[ALSA_UNKNOWN_READ_ERROR]=std::string("Tried to read but couldn't for an unknown reason.");
			errors[ALSA_FULLDUPLEX_PROCESS_ERROR]=std::string("Tried to execute your process function, but it returned error.");
			errors[ALSA_FRAME_MISMATCH_ERROR]=std::string("Frame size mismatch. ");
			errors[ALSA_NO_CHANNELS_ERROR]=std::string("No channels found. ");
			errors[ALSA_YOUR_PROCESS_FN_ERROR]=std::string("Your process function returned error. ");
			errors[ALSA_FORMAT_MISMATCH_ERROR]=std::string("The bits in the words don't match. ");
			errors[ALSA_MIXER_NOT_OPEN_ERROR]=std::string("The mixer device isn't open, open first.");
			errors[ALSA_MIXER_QUERY_ERROR]=std::string("The mixer query failed.");
			errors[ALSA_MIXER_FIND_ELEMENT_ERROR]=std::string("Failed to find the element.");
			errors[ALSA_MIXER_WRONG_ELEMENT_ERROR]=std::string("The two mixer elements are different.");
			errors[ALSA_MIXER_NO_CHANNEL_ERROR]=std::string("That channel couldn't be found.");
			errors[ALSA_MIXER_NO_PLAYBACK_VOL_ERROR]=std::string("That mixer element doesn't have a playback vol control.");
			errors[ALSA_MIXER_NO_CAPTURE_VOL_ERROR]=std::string("That mixer element doesn't have a capture vol control.");
			errors[ALSA_CONTROL_NOT_OPEN_ERROR]=std::string("The control device isn't open, open first.");
			errors[ALSA_SCHED_PRIORITY_ERROR]=std::string("When setting the thread priority.");
			errors[ALSA_SCHED_POLICY_ERROR]=std::string("When setting the thread policy.");
			errors[ALSA_MIXER_NO_ENUM_ERROR]=std::string("That mixer element is not an enum control.");
			errors[ALSA_INFO_NOT_OPEN_ERROR]=std::string("The info structure isn't allocated. Info::getInfo first.");
			errors[ALSA_MIXER_NO_ELEMENT_ERROR]=std::string("The mixer element was null or incorrectly specified.");
			errors[ALSA_CONFIG_NOT_OPEN_ERROR]=std::string("The config file couldn't be opened or wasn't open.");
			errors[ALSA_UNHANDLED_TYPE]=std::string("The type found was not handleable.");

			#endif
		}

		virtual int evaluateError(int errorNum) {
			std::cout<<errorNum<<std::endl;
			if (errorNum>MAX_ERROR_OFFSET || errorNum<MIN_ERROR_OFFSET){
				fprintf(stderr, "%s ", snd_strerror(errorNum));
				return errorNum;
			}
			return Debug::evaluateError(errorNum);
		}

		virtual int evaluateError(int errorNum, std::string append) {
			return Debug::evaluateError(errorNum, append);
		}

		/** Check if an error is ALSA_MIXER_NO_CHANNEL_ERROR
		\param var The error value to compare against
		\return true if var == ALSA_MIXER_NO_CHANNEL_ERROR
		*/
		bool noChannelError(int val){ return val==ALSA_MIXER_NO_CHANNEL_ERROR;}
	};
}
#endif // ALSADEBUG_H
