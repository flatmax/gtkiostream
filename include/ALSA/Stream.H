#ifndef STREAM_H
#define STREAM_H

#include <ALSA/ALSA.H>

namespace ALSA {
  /** C++ object for the ALSA stream either an input or output group of channels of a soundcard.
  */
  class Stream : public virtual Software {

  protected:
    bool block; ///< Whether to block or use NONBLOCK

  public:
    /// Constructor
    Stream() {
      block=1;
    }

    int init(const char *device, snd_pcm_stream_t streamType, bool blockIn) {
      block=blockIn;
      int ret=open(device, streamType, block);
      if (ret<0){
        std::cerr<<"Couldn't open device"<<std::endl;
        return ret;
      }

      // if ((ret=rateResample(1))<0){
      //   std::cerr<<"Couldn't rateResample"<<std::endl;
      //   return ret;
      // }
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
      std::cout<<"Srteam:: fs = "<<startRate<<std::endl;
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
      std::cout<<"opening the device "<<device<<std::endl;
        int ret;
        if ((ret=PCM::open(device, streamType, block))<0)
          return ALSADebug().evaluateError(ret);
        if ((ret=fillParams())<0)
          std::cerr<<"Couldn't fillParams "<<snd_strerror(ret)<<std::endl;
        return ret;
    }

    /* Reopen the stream, set params must be called manually
    \return >= 0 on success
    */
    int reOpen(const char *devName) {
      close();
      return open(devName, SND_PCM_STREAM_PLAYBACK, block);
    }

    /** Set hardware and software params
    \return >= 0 on success
    */
    int setParams() {
      int err=setHWParams();
      if (err < 0) {
        return ALSADebug().evaluateError(err, "Stream:: setParams : Unable to set hw params ");
        return err;
      }

      if ((err = getSWParams()) < 0)
        return ALSADebug().evaluateError(err, "Stream:: setParams : Unable to determine current sParams ");

      if ((err = setSWThreshold(0x7fffffff)) < 0)
        return ALSADebug().evaluateError(err, "Stream:: setParams : Unable to set start threshold mode ");

      snd_pcm_uframes_t val = 4;
      if (block)
        getPeriodSize(&val);

      if ((err=setAvailMin(val))< 0)
        return ALSADebug().evaluateError(err, "Stream:: setParams : Unable to set avail min ");

      if ((err=setSWParams()) < 0)
        return ALSADebug().evaluateError(err, "Stream:: setParams : Unable to set sw params ");
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
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
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
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
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
  };
}
#endif //STREAM_H
