#ifndef FULLDUPLEX_H
#define CAPTURE_H

#include <ALSA/ALSA.H>

namespace ALSA {
	/** Class to operate ALSA in a full duplex mode. The process is write out, read in and process.
	You implement the process method. An example is like so :
	\code
	class FullDuplexTest : public FullDuplex {
		int N; ///< The number of frames
		int ch; ///< The number of channels

		int process(){
			if (inputAudio.rows()!=N || inputAudio.cols()!=ch){ // force resize on the first pass through
				inputAudio.resize(N, ch);
				outputAudio.resize(N, ch);
			}

			outputAudio=inputAudio; // copy the input to output.
			return 0; // return 0 to continue
		}
	public:
		FullDuplexTest(const char*devName, int latency) : FullDuplex(devName){
			ch=2; // use this static number of input and output channels.
			N=latency;
			inputAudio.resize(0,0); // force zero size to ensure resice on the first process.
			outputAudio.resize(0,0);
		}
	};
	\endcode
	*/
	template<typename FRAME_TYPE>
	class FullDuplex : public Capture, public Playback {
		/** write, read and process.
		\returns <0 on error, 0 to continue, >0 to stop
		*/
		int writeReadProcess(){
			int ret=Playback::writeBuf(outputAudio);
			if (ret==0)
				ret=Capture::readBuf(inputAudio);
			if (ret==0)
				ret=process();
			return ret;
		}

		/** Your class must inherit this class and implement the process method.
		The inputAudio and outputAudio variables should be resized to the number of channels
		and frames you want to process. Note that the number of frames must be the same for
		both the inputAudio and outputAudio variables.
		\return <0 on error, 0 to continue and >0 to stop.
		*/
		virtual int process()=0;
protected:
	/// The input audio variable, columns are channels, rows are frames (samples).
	Eigen::Array<FRAME_TYPE, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> inputAudio;
	/// The output audio variable, columns are channels, rows are frames (samples).
	Eigen::Array<FRAME_TYPE, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> outputAudio;

	public:
		/** Constructor using the same device for both capture and playback.
		\param devName The device name to use
		*/
		FullDuplex(const char *devName) : Capture(devName), Playback(devName) {}

		/** Constructor using the different devices for capture and playback.
		\param playDevName The device name to use
		\param captureDevName The device name to use
		*/
		FullDuplex(const char *playDevName, const char *captureDevName) : Capture(captureDevName), Playback(playDevName) {}

		/** Destructor
		*/
		virtual ~FullDuplex(void){}

		/** link the capture and playback devices.
		\return <0 on error.
		*/
		int link(){
			return snd_pcm_link(Playback::getPCM(), Capture::getPCM());
		}

		/** unlink the capture and playback devices.
		\return <0 on error.
		*/
		int unLink(){
			return snd_pcm_unlink(Capture::getPCM());
		}

		/** Begin the read and write process.
		Your process method is called once before starting the ALSA read/write functions.
		This allows you to initialise your member variables as required in a non-realtime fashion.
		This method will set the channels as you require them and prepare the playback/capture hardware/software.
		\return <0 on error, >0 on success.
		*/
		virtual int go(){
			int ret=process(), ret2; // call the user's process method to initialise mamber variables.
			if (ret<0)
				return ALSADebug().evaluateError(ALSA_YOUR_PROCESS_FN_ERROR);

			if (inputAudio.rows()!=outputAudio.rows()) // check for frames mismatch
				return ALSADebug().evaluateError(ALSA_FRAME_MISMATCH_ERROR, "Your process method didn't initialise the inputAudio and outputAudio member variables with the same number of frames.");
			if (inputAudio.cols()==0 || outputAudio.cols()==0) // check for no channels
				return ALSADebug().evaluateError(ALSA_NO_CHANNELS_ERROR, "Your process method didn't initialise the inputAudio and outputAudio member variables with any channels.");

			if (!Playback::prepared()){
				// set the playback and capture channels and period sizes accordingly and set the parameters
				if ((ret=Playback::setChannels(outputAudio.cols()))<0)
					return ALSADebug().evaluateError(ret);
				if ((ret=Capture::setChannels(inputAudio.cols()))<0)
					return ALSADebug().evaluateError(ret);
				if ((ret=Playback::setBufSize(outputAudio.rows()))<0)
					return ALSADebug().evaluateError(ret);
				if ((ret=Capture::setBufSize(inputAudio.rows()))<0)
					return ALSADebug().evaluateError(ret);

				if (sizeof(FRAME_TYPE)!=Playback::getFormatPhysicalWidth()/8)
					return ALSADebug().evaluateError(ALSA_FORMAT_MISMATCH_ERROR, " When comparing the FullDuplex sample type template word against the ALSA format word. ");

				Playback::setParams();
				Capture::setParams();
			}
			// Capture::enableLog();
			// std::cout<<"status"<<std::endl;
			// Capture::dumpStatus();
			// std::cout<<"PCM"<<std::endl;
			// Capture::dumpPCM();
			// std::cout<<"setup"<<std::endl;
			// Capture::dumpSetup();
			// std::cout<<"\n\nHW setup"<<std::endl;
			// Capture::dumpHWSetup();
			// std::cout<<"\nHW params"<<std::endl;
			// Capture::dumpHWParams();
			// std::cout<<"\n\nSW setup"<<std::endl;
			// Capture::dumpSWSetup();
			// std::cout<<"\nSW params"<<std::endl;
			// Capture::dumpSWParams();

			if ((ret=link())<0)
				return ALSADebug().evaluateError(ret);

			while ((ret=writeReadProcess())==0)
				;
			if (Playback::running())
				Playback::drop(); // stop the pcm
			if ((ret2=unLink())<0)
				if (ret>=0)
					return ALSADebug().evaluateError(ret2);
				else
					ALSADebug().evaluateError(ret2);
			return ret;
		}

		/** Reset the params with a full configuration space for both PCMs.
		Note you have to call setAccess, setFormat, setChannels, setSampleRate if you call this.
		\return >= 0 on success
		*/
		int resetParams() {
			int ret=0;
			if (ret=Playback::resetParams()<0)
				return ret;
			return Capture::resetParams();
		}

		/** @see Hardware::setFormat
		Set the word format of the hardware
		*/
		int setFormat(snd_pcm_format_t format) {
			int ret=0;
			if (ret=Playback::setFormat(format)<0)
				return ret;
			return Capture::setFormat(format);
		}

		/** @see Hardware::setAccess
		Set the interleaving of the hardware.
		*/
		int setAccess(snd_pcm_access_t access) {
			int ret=0;
			if (ret=Playback::setAccess(access)<0)
				return ret;
			return Capture::setAccess(access);
		};

		/** Set the sample rate closest to the desired rate.
		@see Hardware::setSampleRate
		\param rrate The desired sample rate.
		\param dir The direction to search in (-1,0,1)
		\return >= 0 on success
		*/
		int setSampleRate(unsigned int rrate, int dir=0) {
			int ret=0;
			if (ret=Playback::setSampleRate(rrate, dir)<0)
				return ret;
			return Capture::setSampleRate(rrate, dir);
		}

		/** Set the buffer size
		\param the desired buffer size
		\return <0 on error.
		*/
		int setBufSize(snd_pcm_uframes_t bufSize) {
			int ret=0;
			if (ret=Playback::setBufSize(bufSize)<0)
				return ret;
			return Capture::setBufSize(bufSize);
		}

		/** Set the number of channels
		\param cnt The number of channels.
		\return >= 0 on success
		*/
		int setChannels(unsigned int cnt) {
			int ret=0;
			if (ret=Playback::setChannels(cnt)<0)
				return ret;
			return Capture::setChannels(cnt);
		}
	};
}
#endif //FULLDUPLEX_H
