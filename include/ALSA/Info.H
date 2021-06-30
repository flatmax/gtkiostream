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
#ifndef INFO_H
#define INFO_H

#include <ALSA/ALSA.H>

namespace ALSA {

	/** Infomation class for the ALSA lib, representing one sound card.
	*/
	class Info  {
		snd_pcm_info_t *info;
	public:
		Info(){
			info=NULL;
		}

		~Info(){
			if (info)
				snd_pcm_info_free(info);
			info=NULL;
		}

		/** Given a pcm device, populate info
		\param pcm the device to get information from
		\returns 0 on success
		*/
		int getInfo(PCM &pcm){
			PCM_NOT_OPEN_CHECK(pcm.getPCM()) // check pcm is open
			if (info==NULL)
				if (snd_pcm_info_malloc(&info)<0)
					ALSADebug().evaluateError(-ENOMEM,"Couldn't malloc info.");
			int ret=snd_pcm_info(pcm.getPCM(), info);
			if (ret<0)
				ALSADebug().evaluateError(ret);
			return ret;
		}

		/** Get the card number
		\returns Card on success, <0 on error
		*/
		int getCard(){
			INFO_NOT_OPEN_CHECK(info);
			int ret=snd_pcm_info_get_card(info);
			if (ret<0)
				ALSADebug().evaluateError(ret);
			return ret;
		}

		/** Get the device number
		\returns Device number
		*/
		unsigned int getDevice(){
			INFO_NOT_OPEN_CHECK_TYPED(info, unsigned int);
			return snd_pcm_info_get_device(info);
		}

		/** Get the sub-device number
		\returns Sub-device number
		*/
		unsigned int getSubDevice(){
			INFO_NOT_OPEN_CHECK_TYPED(info, unsigned int);
			return snd_pcm_info_get_subdevice(info);
		}

		/** Get the short ID string
		\returns Short ID string
		*/
		const char* getID(){
			INFO_NOT_OPEN_CHECK_STR(info, ALSADebug());
			return snd_pcm_info_get_id(info);
		}

		/** Get the name of the device
		\returns The device name
		*/
		const char* getName(){
			INFO_NOT_OPEN_CHECK_STR(info, ALSADebug());
			return snd_pcm_info_get_name(info);
		}

		/** Get the sub-device name of the device
		\returns The sub-device name
		*/
		const char* getSubDeviceName(){
			INFO_NOT_OPEN_CHECK_STR(info, ALSADebug());
			return snd_pcm_info_get_subdevice_name(info);
		}
	};
}
#endif //INFO_H
