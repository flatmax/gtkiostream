#ifndef ALSA_H
#define ALSA_H

#include <alsa/asoundlib.h>
#include <iostream>

namespace ALSA {

// #define ALSA_OPEN_ERROR -1+JACK_ERROR_OFFSET ///< error when trying to connect network ports automatically.

// class JackDebug : public Debug {
// public:
//     JackDebug(void) {
// #ifndef NDEBUG
//         errors[JACK_SERVER_NOT_FOUND_ERROR]=string("The jack server wasn't found");
// #endif
//     }
// };


#define ALSA_DEFAULT_START_FS 48000
#define ALSA_DEFAULT_FORMAT SND_PCM_FORMAT_S16_LE
#define ALSA_DEFAULT_CHANNELS 2

class PCM {
protected:
    snd_pcm_t *handle; ///< PCM handle
public:
    virtual snd_pcm_t *getPCM(){
        return handle;
    }

    /** Get a pointer to the handle pointer of the stream
    \return A pointer to the handle pointer
    */
    snd_pcm_t **getPCMP(){
        return &handle;
    }


};

/** Hardware class for the ALSA lib, representing one sound card.
*/
class Hardware : public PCM {
    snd_pcm_hw_params_t *hParams; ///< PCM params
protected:

    /** Fill params with a full configuration space for a PCM.
    \return >= 0 on success
    */
    int fillParams() {
        return snd_pcm_hw_params_any(getPCM(), hParams);
    }

    /** Set the hardware params
    \return >= 0 on success
    */
    int setHWParams() {
        return snd_pcm_hw_params(getPCM(), hParams);
    }


public:
    Hardware() {
        hParams=NULL;
        snd_pcm_hw_params_malloc(&hParams);
        if (!hParams)
            assert("Couldn't malloc HW params");
    }

    virtual ~Hardware() {
        if (hParams)
            snd_pcm_hw_params_free(hParams);
        hParams=NULL;
    }

    /** Copy the input hw_params to the class
    \param hParamsIn the hw_params to copy
    */
    void copyFrom(snd_pcm_hw_params_t *hParamsIn){
        snd_pcm_hw_params_copy(hParams, hParamsIn);
    }

    /** Alter the sample rate resample.
    \param state True to resample and fals to not resample.
    \return >= 0 on success
    */
    int rateResample(unsigned int state) {
        return snd_pcm_hw_params_set_rate_resample(getPCM(), hParams, state);
    }

    /** Set the type of channel layout in the buffer
    \param access is the type of channel layout, one of
    SND_PCM_ACCESS_MMAP_INTERLEAVED  mmap access with simple interleaved channels
    SND_PCM_ACCESS_MMAP_NONINTERLEAVED  mmap access with simple non interleaved channels
    SND_PCM_ACCESS_MMAP_COMPLEX mmap access with complex placement
    SND_PCM_ACCESS_RW_INTERLEAVED  snd_pcm_readi/snd_pcm_writei access
    SND_PCM_ACCESS_RW_NONINTERLEAVED snd_pcm_readn/snd_pcm_writen access
    \return >= 0 on success
    */
    int setAccess(snd_pcm_access_t access) {
        return snd_pcm_hw_params_set_access(getPCM(), hParams, access);
    }

    /** Set the sample type
    \param format is the sample type
    \return >= 0 on success
    SND_PCM_FORMAT_S8 Signed 8 bit
    SND_PCM_FORMAT_U8 Unsigned 8 bit
    SND_PCM_FORMAT_S16_LE Signed 16 bit Little Endian
    SND_PCM_FORMAT_S16_BE Signed 16 bit Big Endian
    SND_PCM_FORMAT_U16_LE Unsigned 16 bit Little Endian
    SND_PCM_FORMAT_U16_BE Unsigned 16 bit Big Endian
    SND_PCM_FORMAT_S24_LE Signed 24 bit Little Endian using low three bytes in 32-bit word
    SND_PCM_FORMAT_S24_BE Signed 24 bit Big Endian using low three bytes in 32-bit word
    SND_PCM_FORMAT_U24_LE Unsigned 24 bit Little Endian using low three bytes in 32-bit word
    SND_PCM_FORMAT_U24_BE Unsigned 24 bit Big Endian using low three bytes in 32-bit word
    SND_PCM_FORMAT_S32_LE Signed 32 bit Little Endian
    SND_PCM_FORMAT_S32_BE Signed 32 bit Big Endian
    SND_PCM_FORMAT_U32_LE Unsigned 32 bit Little Endian
    SND_PCM_FORMAT_U32_BE Unsigned 32 bit Big Endian
    SND_PCM_FORMAT_FLOAT_LE Float 32 bit Little Endian, Range -1.0 to 1.0
    SND_PCM_FORMAT_FLOAT_BE Float 32 bit Big Endian, Range -1.0 to 1.0
    SND_PCM_FORMAT_FLOAT64_LE Float 64 bit Little Endian, Range -1.0 to 1.0
    SND_PCM_FORMAT_FLOAT64_BE Float 64 bit Big Endian, Range -1.0 to 1.0
    SND_PCM_FORMAT_IEC958_SUBFRAME_LE IEC-958 Little Endian
    SND_PCM_FORMAT_IEC958_SUBFRAME_BE IEC-958 Big Endian
    SND_PCM_FORMAT_MU_LAW Mu-Law
    SND_PCM_FORMAT_A_LAW A-Law
    SND_PCM_FORMAT_IMA_ADPCM Ima-ADPCM
    SND_PCM_FORMAT_MPEG MPEG
    SND_PCM_FORMAT_GSM GSM
    SND_PCM_FORMAT_SPECIAL Special
    SND_PCM_FORMAT_S24_3LE Signed 24bit Little Endian in 3bytes format
    SND_PCM_FORMAT_S24_3BE Signed 24bit Big Endian in 3bytes format
    SND_PCM_FORMAT_U24_3LE Unsigned 24bit Little Endian in 3bytes format
    SND_PCM_FORMAT_U24_3BE Unsigned 24bit Big Endian in 3bytes format
    SND_PCM_FORMAT_S20_3LE Signed 20bit Little Endian in 3bytes format
    SND_PCM_FORMAT_S20_3BE Signed 20bit Big Endian in 3bytes format
    SND_PCM_FORMAT_U20_3LE Unsigned 20bit Little Endian in 3bytes format
    SND_PCM_FORMAT_U20_3BE Unsigned 20bit Big Endian in 3bytes format
    SND_PCM_FORMAT_S18_3LE Signed 18bit Little Endian in 3bytes format
    SND_PCM_FORMAT_S18_3BE Signed 18bit Big Endian in 3bytes format
    SND_PCM_FORMAT_U18_3LE Unsigned 18bit Little Endian in 3bytes format
    SND_PCM_FORMAT_U18_3BE Unsigned 18bit Big Endian in 3bytes format
    SND_PCM_FORMAT_G723_24 G.723 (ADPCM) 24 kbit/s, 8 samples in 3 bytes
    SND_PCM_FORMAT_G723_24_1B G.723 (ADPCM) 24 kbit/s, 1 sample in 1 byte
    SND_PCM_FORMAT_G723_40 G.723 (ADPCM) 40 kbit/s, 8 samples in 3 bytes
    SND_PCM_FORMAT_G723_40_1B G.723 (ADPCM) 40 kbit/s, 1 sample in 1 byte
    SND_PCM_FORMAT_DSD_U8 Direct Stream Digital (DSD) in 1-byte samples (x8)
    SND_PCM_FORMAT_DSD_U16_LE Direct Stream Digital (DSD) in 2-byte samples (x16)

    SND_PCM_FORMAT_S16 Signed 16 bit CPU endian
    SND_PCM_FORMAT_U16 Unsigned 16 bit CPU endian
    SND_PCM_FORMAT_S24 Signed 24 bit CPU endian
    SND_PCM_FORMAT_U24 Unsigned 24 bit CPU endian
    SND_PCM_FORMAT_S32 Signed 32 bit CPU endian
    SND_PCM_FORMAT_U32 Unsigned 32 bit CPU endian
    SND_PCM_FORMAT_FLOAT Float 32 bit CPU endian
    SND_PCM_FORMAT_FLOAT64 Float 64 bit CPU endian
    SND_PCM_FORMAT_IEC958_SUBFRAME IEC-958 CPU Endian
    */
    int setFormat(snd_pcm_format_t format) {
        return snd_pcm_hw_params_set_format(getPCM(), hParams, format);
    }

    /** Get the format from a configuration space
     \param params Configuration space
     \param format returned format
     \return format otherwise a negative error code if the configuration space does not contain a single value
     */
    int getFormat(snd_pcm_format_t &format){
        return snd_pcm_hw_params_get_format(hParams, &format);
    }

    /** Set the number of channels
    \param cnt The number of channels.
    \return >= 0 on success
    */
    int setChannels(unsigned int cnt) {
        return snd_pcm_hw_params_set_channels(getPCM(), hParams, cnt);
    }

    /** Get the channel count
    \return >= 0 on success returns the channel count
    */
    int getChannels() {
        unsigned int cnt;
        int err=snd_pcm_hw_params_get_channels(hParams, &cnt);
        if (err<0)
            return err;
        else
            return cnt;
    }

    /** Set the sample rate closest to the desired rate.
    \param rrate The desired sample rate.
    \param dir The direction to search in (-1,0,1)
    \return >= 0 on success
    */
    int setSampleRate(unsigned int rrate, int dir=0) {
        return snd_pcm_hw_params_set_rate_near(getPCM(), hParams, &rrate, &dir);
    }

    /** Get the sample rate
    \param rate The current sample rate.
    \param dir The direction to search in (-1,0,1)
    \return >= 0 on success
    */
    int getSampleRate(int dir=0) {
        unsigned int rate;
        int err=snd_pcm_hw_params_get_rate(hParams, &rate, &dir);
        if (err<0)
            return err;
        else
            return rate;
    }

    /** Get the period size
    \param p Returned approximate period size in frames
    \param dir The direction to search in (-1,0,1)
    \return >= 0 on success
    */
    int getPeriodSize(snd_pcm_uframes_t *p, int *dir=NULL) {
        return snd_pcm_hw_params_get_period_size(hParams, p, dir);
    }

    /** Set the period size
    \param p approximate period size in frames
    \param dir The direction to search in (-1,0,1)
    \return >= 0 on success
    */
    int setPeriodSize(snd_pcm_uframes_t *p, int *dir=0) {
        return snd_pcm_hw_params_set_period_size_near(getPCM(), hParams, p, dir);
    }

    int setBufSize(snd_pcm_uframes_t bufSize) {
        snd_pcm_uframes_t periodSize = bufSize * 2;

        int err = snd_pcm_hw_params_set_buffer_size_near(getPCM(), hParams, &periodSize);
        if (err < 0) {
            std::cerr<<"Unable to set buffer size "<<bufSize * 2<<" "<< snd_strerror(err)<<std::endl;
            return err;
        }
        periodSize /= 2;
        if ((err = setPeriodSize(&periodSize)) < 0)
            std::cerr<<"Unable to set period size "<<periodSize<<" "<< snd_strerror(err)<<std::endl;
    }

    const char *formatDescription(const snd_pcm_format_t format) const {
        return snd_pcm_format_description(format);
    }
};

class Software : public Hardware {
    snd_pcm_sw_params_t *sParams; ///< PCM software params
public:
    Software(void){
        sParams=NULL;
        snd_pcm_sw_params_malloc(&sParams);
        if (!sParams)
            assert("Couldn't malloc SW params");
    }

    virtual ~Software(void){
        if (sParams)
            snd_pcm_sw_params_free(sParams);
        sParams=NULL;
    }

    /** Get the software params
    \return >= 0 on success
    */
    int getSWParams() {
        return snd_pcm_sw_params_current(getPCM(), sParams);
    }

    /** Set the software params
    \return >= 0 on success
    */
    int setSWParams() {
        return snd_pcm_sw_params(getPCM(), sParams);
    }

    /** Set the start threshold
    \param thresh Start threshold in frames
    \return >= 0 on success
    */
    int setSWThreshold(snd_pcm_uframes_t thresh) {
        return snd_pcm_sw_params_set_start_threshold(getPCM(), sParams, thresh);
    }

    /** Set avail min inside a software configuration container
    \param cnt Minimum avail frames to consider PCM ready
    \return >= 0 on success
    */
    int setAvailMin(snd_pcm_uframes_t cnt) {
        return snd_pcm_sw_params_set_avail_min(getPCM(), sParams, cnt);
    }

    /** Return the address to start reading/writing to give an ALSA areas type
    \param areas The ALSA provided snd_pcm_channel_area_t type
    \return The pointer to start reading or writing to/from
    */
    void *getAddress(const snd_pcm_channel_area_t *areas, snd_pcm_uframes_t offset) const {
        return (areas->addr+(areas->first+offset*areas->step)/8);
    }

    /** Return the number of channels in an ALSA provided areas type
    \param areas The ALSA provided snd_pcm_channel_area_t type
    \tparam SAMPLE_TYPE The type of the samples to derive the channel count for
    \return The number of channels
    */
    template<typename SAMPLE_TYPE>
    int getChannelCount(const snd_pcm_channel_area_t *areas) const {
        return areas->step/sizeof(SAMPLE_TYPE)/8;;
    }
};

/** C++ object for the ALSA stream either an input or output group of channels of a soundcard.
*/
class Stream : public virtual Software {

protected:
    bool block; ///< Whether to block or use NONBLOCK

public:
    /// Constructor
    Stream() {
    }

    int init(const char *device, snd_pcm_stream_t streamType, bool blockIn) {
    	block=blockIn;
    	int ret=open(device, streamType, block);
        if (ret<0){
            std::cerr<<"Couldn't open device"<<std::endl;
        	return ret;
        }
        if ((ret=fillParams())<0){
            std::cerr<<"Couldn't fillParams"<<std::endl;
        	return ret;
        }

        if ((ret=rateResample(1))<0){
            std::cerr<<"Couldn't rateResample"<<std::endl;
        	return ret;
        }
        if ((ret=setAccess(SND_PCM_ACCESS_RW_INTERLEAVED))<0){
            std::cerr<<"Couldn't setAccess"<<std::endl;
        	return ret;
        }
        if ((ret=setFormat(ALSA_DEFAULT_FORMAT))<0){
            std::cerr<<"Couldn't setFormat"<<std::endl;
        	return ret;
        }
        if ((ret=setChannels(ALSA_DEFAULT_CHANNELS))<0){
            std::cerr<<"Couldn't setChannels"<<std::endl;
        	return ret;
        }

        unsigned int startRate=ALSA_DEFAULT_START_FS;
        if ((ret=setSampleRate(startRate))<0){
            std::cerr<<"Couldn't setSampleRate"<<std::endl;
        	return ret;
        }
        std::cout<<"Steam:: fs = "<<startRate<<std::endl;
    }

    /// Destructor
    virtual ~Stream(){
        close();
    }

	/**
	 * \brief get name of PCM sample format
	 * \param format PCM sample format
	 * \return ascii name of PCM sample format
	 */
 	const char *getFormatName(const snd_pcm_format_t format){
 		return snd_pcm_format_name(format);
 	}

    /** Open the stream
    \param device The name of the device
    \param streamType SND_PCM_STREAM_PLAYBACK or SND_PCM_STREAM_CAPTURE
    \param block Whether to open non blocking
    \return >= 0 on success
    */
    int open(const char *device, snd_pcm_stream_t streamType, const int block) {
        return snd_pcm_open(getPCMP(), device, streamType, block);
    }

    /** Close the stream
    \return >= 0 on success
    */
    int close(){
        return snd_pcm_close(getPCM());
    }

    /** Set hardware and software params
    \return >= 0 on success
    */
    int setParams() {
        int err=setHWParams();
        if (err < 0) {
            std::cerr<<"Stream:: setParams : Unable to set hw params "<< snd_strerror(err)<<std::endl;
            return err;
        }

        if ((err = getSWParams()) < 0) {
            std::cerr<<"Stream:: setParams : Unable to determine current sParams "<< snd_strerror(err)<<std::endl;
            return err;
        }

        if ((err = setSWThreshold(0x7fffffff)) < 0) {
            std::cerr<<"Stream:: setParams : Unable to set start threshold mode "<< snd_strerror(err)<<std::endl;
            return err;
        }

        snd_pcm_uframes_t val = 4;
        if (block)
            getPeriodSize(&val);

        if ((err=setAvailMin(val))< 0) {
            std::cerr<<"Stream:: setParams : Unable to set avail min "<< snd_strerror(err)<<std::endl;
            return err;
        }

        if ((err=setSWParams()) < 0)
            std::cerr<<"Stream:: setParams : Unable to set sw params "<< snd_strerror(err)<<std::endl;
        return err;
    }

	/**
	 * \brief Silence a PCM samples buffer
	 * \param data Buffer
	 * \param samples Samples count
	 * \return 0 if successful or a negative error code
	 */
	int setSilence(void *data, unsigned int samples){
		snd_pcm_format_t format;
		int ret=getFormat(format);
		if (ret<0)
			return ret;
		return snd_pcm_format_set_silence(format, data, samples);
	}

    /** Link this stream to another stream
    \param s A stream to link to
    \return 0 on success otherwise a negative error code
    */
    int link(Stream &s){
        return snd_pcm_link(getPCM(), *s.getPCMP());
    }

    /** Wait for data with a timeout
    \param timeOut maximum time in milliseconds to wait,
        a negative value means infinity
    \return a positive value on success otherwise a negative error code
         (-EPIPE for the xrun and -ESTRPIPE for the suspended status,
          others for general errors)
    \retval 0 timeout occurred
	\retval 1 PCM stream is ready for I/O
    */
    int wait(int timeOut=1000) {
        return snd_pcm_wait(getPCM(), timeOut);
    }

    /** Return nominal bits per a PCM sample
     \return bits per sample, a negative error code if not applicable
     */
     int getFormatBits(){
     	snd_pcm_format_t format;
     	int ret=getFormat(format);
     	if (ret>=0)
	        return snd_pcm_format_width(format);
	    return ret;
     }

    /** Dump PCM info.
    Outputs on stdout
    */
    /*void dump(){
        snd_pcm_dump(getPCM(), stdout);
    }*/
};


/** The input channels of this sound card
*/
class Capture : public Stream {
public:
    Capture() {
        int err=init("default", SND_PCM_STREAM_CAPTURE, block ? 0 : SND_PCM_NONBLOCK);
        if (err < 0) {
        	std::cout<<err<<std::endl;
            std::cerr<<"Capture :: Capture : open error: "<< snd_strerror(err)<<std::endl;
            assert("open error");
        }
        if ((err=setParams())<0) {
            std::cerr<<"Capture :: Capture : set params error: "<< snd_strerror(err)<<std::endl;
            assert("set params error");
        }
    }

    size_t writeBuf(char *buffer, size_t len, size_t &frames){
		size_t r;

		while (len > 0) {
			r = snd_pcm_writei(getPCM(), buffer, len);
			if (r == -EAGAIN)
				continue;
			// printf("write = %li\n", r);
			if (r < 0)
				return r;
			// showstat(getPCM(), 0);
			buffer += r * 4;
			len -= r;
			frames += r;
		}
		return 0;
	}
	
};

/** The output channels of this sound card
*/
class Playback : public Stream {
    public:
        Playback() {
            int err=init("default", SND_PCM_STREAM_PLAYBACK, block ? 0 : SND_PCM_NONBLOCK);
            if (err < 0) {
	        	std::cout<<err<<std::endl;
                std::cerr<<"Playback :: Playback : open error: "<< snd_strerror(err)<<std::endl;
                assert("open error");
            }
            if ((err=setParams())<0) {
                std::cerr<<"Playback :: Playback : set params error: "<< snd_strerror(err)<<std::endl;
                assert("set params error");
            }
        }

        size_t readbuf(char *buffer, size_t len, size_t &frames, size_t &max){
			size_t r;

			if (!block) {
				do {
					r = snd_pcm_readi(getPCM(), buffer, len);
				} while (r == -EAGAIN);
				if (r > 0) {
					frames += r;
					if (max < r)
						max = r;
				}
				// printf("read = %li\n", r);
			} else {
				int frame_bytes = (getFormatBits() / 8) * getChannels();
				do {
					r = snd_pcm_readi(getPCM(), buffer, len);
					if (r > 0) {
						buffer += r * frame_bytes;
						len -= r;
						frames += r;
						if (max < r)
							max = r;
					}
					// printf("r = %li, len = %li\n", r, len);
				} while (r >= 1 && len > 0);
			}
			// showstat(getPCM(), 0);
			return r;
		}
};
};
#endif // ALSA_H
