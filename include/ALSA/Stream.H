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
#ifndef STREAM_H
#define STREAM_H

#include <pthread.h>

#include <ALSA/ALSA.H>

namespace ALSA {
  /** C++ object for the ALSA stream either an input or output group of channels of a soundcard.
  */
  class Stream : public Software {

  protected:
    bool block; ///< Whether to block or use NONBLOCK

  public:
    /// Constructor
    Stream() {
      block=1;
    }

    int init(const char *device, snd_pcm_stream_t streamType, bool blockIn) {
      block=blockIn;
      int ret=open(device, streamType, block ? 0 : SND_PCM_NONBLOCK);
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
      return 0;
    }

    /// Destructor
    virtual ~Stream(){
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

    /** Set the current thread's priority
    \param priority <0 implies maximum priority, otherwise must be between sched_get_priority_max and sched_get_priority_min
    \return 0 on success, error code otherwise
    */
    int changeThreadPriority(int priority){
      int ret;
      pthread_t thisThread = pthread_self(); // get the current thread
      struct sched_param origParams, params;
      int origPolicy, policy = SCHED_FIFO, newPolicy=0;

      if ((ret = pthread_getschedparam(thisThread, &origPolicy, &origParams))!=0)
        return ALSADebug().evaluateError(ret, "when trying to pthread_getschedparam\n");
      printf("ALSA::Stream::changeThreadPriority : Current thread policy %d and priority %d\n", origPolicy, origParams.sched_priority);

      if (priority<0) //maximum priority
        params.sched_priority = sched_get_priority_max(policy);
      else
        params.sched_priority = priority;

      if (params.sched_priority>sched_get_priority_max(policy))
        return ALSADebug().evaluateError(ALSA_SCHED_PRIORITY_ERROR, "requested priority is too high\n");
      if (params.sched_priority<sched_get_priority_min(policy))
        return ALSADebug().evaluateError(ALSA_SCHED_PRIORITY_ERROR, "requested priority is too low\n");

      if ((ret = pthread_setschedparam(thisThread, policy, &params))!=0)
        return ALSADebug().evaluateError(ret, "when trying to pthread_setschedparam - are you su or do you have permission to set this priority?\n");
      if ((ret = pthread_getschedparam(thisThread, &newPolicy, &params))!=0)
        return ALSADebug().evaluateError(ret, "when trying to pthread_getschedparam\n");
      if(policy != newPolicy)
        return ALSADebug().evaluateError(ALSA_SCHED_POLICY_ERROR, "requested scheduler policy is not correctly set\n");
      printf("ALSA::Stream::changeThreadPriority : New thread priority changed to %d\n", params.sched_priority);

      return 0;
    }
  };
}
#endif //STREAM_H
