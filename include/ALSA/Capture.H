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
#ifndef CAPTURE_H
#define CAPTURE_H

#include <ALSA/ALSA.H>

namespace ALSA {
	/** The input channels of this sound card
	*/
	class Capture : public Stream {
		void init(const char *devName){
			int err=Stream::init(devName, SND_PCM_STREAM_CAPTURE, block ? 0 : SND_PCM_NONBLOCK);
			if (err < 0) {
				std::cout<<err<<std::endl;
				std::cerr<<"Capture :: Capture : open error: "<< snd_strerror(err)<<std::endl;
				assert("open error");
			}
		}
	public:
		/** Constructor specifying the device name
		\param devName The device name to open.
		*/
		Capture(const char *devName) : Stream() {
			init(devName);
		};

		/// Constructor
		Capture() {
			init("default");
		};

		/** Read data from the PCM device - inverleaved version
		\param buffer The audio buffer to read into
		\param len The number of audio frames to write
		*/
		int readBuf(char *buffer, size_t len){
			PCM_NOT_OPEN_CHECK_NO_PRINT(getPCM(), int) // check pcm is open
			int ret=0;
			if (prepared())
				if ((ret=start())<0)
					return ALSADebug().evaluateError(ret);

			// PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
			int bytes_per_frame = getFormatPhysicalWidth() * getChannels()/8;
			while ((len-=ret)>0){
				ret = snd_pcm_readi(getPCM(), buffer, len);
				if (ret<0)
					switch (ret){
						case -EAGAIN: // try again
							ret=0;
							break;
						case -EBADFD:
							printf("Capture::readbuf : PCM state %s\n", getStateName());
							return ALSADebug().evaluateError(ret, "reading failed because pcm is not in the correct state\n");
						case -EPIPE:
								ALSADebug().evaluateError(ret," writeBuf overrun\n");
							return ALSADebug().evaluateError(ret);
						case -ESTRPIPE:
							return ALSADebug().evaluateError(ret);
						default:
							std::cout<<"read error "<<ret<<'\n';
							return ALSADebug().evaluateError(ALSA_UNKNOWN_READ_ERROR);
					}
				if (ret==0 && block!=0)
					wait();
				buffer+= ret*bytes_per_frame;
			}
			return 0;
		}

		/** Method to read the audio buffer from the PCM device. The number of frames
		read is taken from audioData.rows().
		\param audioData The audio buffer to read data into.
		*/
		template <typename Derived>
		int readBuf(const Eigen::DenseBase<Derived> &audioData){
			return readBuf((char*)&audioData(0,0), audioData.rows());
		}

		/** Using known parameters from the Eigen densebase, read the audio in.
		@param audioData One channel per column
		@return A reference to this instance for further use.
		*/
		template <typename Derived>
		Capture& operator>>(const Eigen::DenseBase<Derived> &audioData){
			if (readBuf(audioData)<0)
				throw;
			return *this;
		}
	};
}
#endif //CAPTURE_H
