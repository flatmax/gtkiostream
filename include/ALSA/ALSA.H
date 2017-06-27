#ifndef ALSA_H
#define ALSA_H

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-attributes"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <Eigen/Dense>
#pragma GCC diagnostic pop
#include <alsa/asoundlib.h>
#include <iostream>
#include <Debug.H>

namespace ALSA {
	#define ALSA_PCM_NOT_OPEN_ERROR -1+ALSA_ERROR_OFFSET ///< error when pcm pointer is null
	#define ALSA_UNKNOWN_READ_ERROR -2+ALSA_ERROR_OFFSET ///< error when reading and an unknown code is retured
	#define ALSA_FULLDUPLEX_PROCESS_ERROR -3+ALSA_ERROR_OFFSET ///< error when processing the full duplex method.
	#define ALSA_FRAME_MISMATCH_ERROR -4+ALSA_ERROR_OFFSET ///< error when frame sizes are incorrect
	#define ALSA_NO_CHANNELS_ERROR -5+ALSA_ERROR_OFFSET ///< error when channel cnt is zero
	#define ALSA_YOUR_PROCESS_FN_ERROR -6+ALSA_ERROR_OFFSET ///< error when calling the user's process function
	#define ALSA_FORMAT_MISMATCH_ERROR -7+ALSA_ERROR_OFFSET ///< error when comparing bits sizes of two words

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
	};

	#define PCM_NOT_OPEN_CHECK_NO_PRINT(pcm, type) {if (!pcm) return (type)ALSADebug().evaluateError(ALSA_PCM_NOT_OPEN_ERROR);}
	#define PCM_NOT_OPEN_CHECK_TYPED(pcm, type) {std::cout<<"\t\tfunc: "<<__func__<<'\n'; PCM_NOT_OPEN_CHECK_NO_PRINT(pcm, type)}
	#define PCM_NOT_OPEN_CHECK(pcm) PCM_NOT_OPEN_CHECK_TYPED(pcm, int)
	#define PCM_NOT_OPEN_CHECK_STRING(pcm) {std::cout<<"\t\tfunc: "<<__func__<<'\n'; if (!pcm) return "PCM not open error";}

	#define ALSA_DEFAULT_START_FS 48000
	#define ALSA_DEFAULT_FORMAT SND_PCM_FORMAT_S16_LE
	#define ALSA_DEFAULT_CHANNELS 2
}

#include <ALSA/PCM.H>
#include <ALSA/Hardware.H>
#include <ALSA/Software.H>
#include <ALSA/Stream.H>
#include <ALSA/Playback.H>
#include <ALSA/Capture.H>
#include <ALSA/FullDuplex.H>
#endif // ALSA_H
