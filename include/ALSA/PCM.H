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
#ifndef PCM_H
#define PCM_H

#include <ALSA/ALSA.H>

namespace ALSA {

  /** Class to handle an ALSA pcm object
  */
  class PCM {
  protected:
    snd_output_t *log; ///< The log stream if enabled
    snd_pcm_t *handle; ///< PCM handle
  public:
    PCM(){
      log=NULL;
      handle=NULL;
    }

    virtual ~PCM(){
      if (log)
        snd_output_close(log);
      close();
    }

    virtual snd_pcm_t *getPCM(){
      return handle;
    }

    /** Get a pointer to the handle pointer of the stream
    \return A pointer to the handle pointer
    */
    snd_pcm_t **getPCMP(){
      return &handle;
    }


    /** Returns the type of the PCM
    \return snd_pcm_type_t the type of the PCM
    */
    snd_pcm_type_t getPCMType(){
      PCM_NOT_OPEN_CHECK_TYPED(getPCM(), snd_pcm_type_t) // check pcm is open
      return snd_pcm_type(getPCM());
    }

    /** Open the stream
    \param device The name of the device
    \param streamType SND_PCM_STREAM_PLAYBACK or SND_PCM_STREAM_CAPTURE
    \param block Whether to open non blocking
    \return >= 0 on success
    */
    int open(const char *device, snd_pcm_stream_t streamType, const int block) {
      std::cout<<"opening the device "<<device<<std::endl;
        int ret=snd_pcm_open(getPCMP(), device, streamType, block);
        if (ret<0)
          std::cerr<<"Couldn't open the device "<<snd_strerror(ret)<<std::endl;
        return ret;
    }

    /** Close the stream
    \return >= 0 on success
    */
    int close(){
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
      int ret=0;
      drop();
      snd_pcm_hw_free(getPCM());
      ret=snd_pcm_close(getPCM());
      *getPCMP()=NULL;
      return ret;
    }

    /** Reopen the stream
    \param block Whether to open non blocking
    \return >= 0 on success
    */
    int reopen(const char *device, const int block){
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
      snd_pcm_stream_t streamType=snd_pcm_stream(getPCM());
      std::cout<<"opening the device "<<device<<std::endl;
      int ret=close();
      if (ret<0)
        std::cerr<<"Couldn't close the device "<<snd_strerror(ret)<<std::endl;
      return open(device, streamType, block);
    }

    /** Stops the PCM dropping frames.
    \return <0 on error
    */
    int drop(){
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
      int ret=0;
      if (running())
        ret=snd_pcm_drop(getPCM());
      else
        std::cout<<"PCM::drop can't drop, not running"<<std::endl;
      return ret;
    }

    /** Stops the PCM allowing it to finish draining frames.
    \return <0 on error
    */
    int drain(){
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
      int ret=0;
      if (running())
        ret=snd_pcm_drain(getPCM());
      else
        std::cout<<"PCM::drain can't drain, not running"<<std::endl;
      return ret;
    }

    /** Resets the PCM.
    \return <0 on error
    */
    int reset(){
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
      return snd_pcm_reset(getPCM());
    }

    /** Prepares the PCM (e.g. after xrun).
    \return <0 on error
    */
    int prepare(){
      PCM_NOT_OPEN_CHECK_NO_PRINT(getPCM(), bool) // check pcm is open
      return snd_pcm_prepare(getPCM());
    }

    /** Recovers the PCM (e.g. after -EINTR, -EPIPE, -ESTRPIPE).
    \return <0 on error
    */
    int recover(int err){
      PCM_NOT_OPEN_CHECK_NO_PRINT(getPCM(), int) // check pcm is open
      int silent=0;
      return snd_pcm_recover(getPCM(), err, silent);
    }

    /** How many samples available ?
    \return <0 pon error. Return number of frames ready to be read (capture) / written (playback)
    */
    int avail(){
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
      return snd_pcm_avail(getPCM());
    }

    /** How many samples available ?
    \return <0 pon error. Return number of frames ready to be read (capture) / written (playback)
    */
    int availUpdate(){
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
      return snd_pcm_avail_update(getPCM());
    }

    void enableLog(){
      snd_output_stdio_attach(&log, stdout, 0);
    }

    int logEnabled(){
      if (log)
        return 1;
      else
        printf("errorlogging not enabled, call enableLog first.\n");
      return 0;
    }

    int dumpStatus(){
      int err=0;
      if (logEnabled()){
        snd_pcm_status_t *status;
        snd_pcm_status_alloca(&status);
        if ((err = snd_pcm_status(getPCM(), status)) < 0)
                printf("Stream status error: %s\n", snd_strerror(err));
        else
          snd_pcm_status_dump(status, log);
      }
      return err;
    }

    int dumpPCM(){
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
      int ret=0;
      if (!logEnabled())
        return ret;
      return snd_pcm_dump(getPCM(), log);
    }

    int dumpSetup(){
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
      int ret=0;
      if (!logEnabled())
        return ret;
      return snd_pcm_dump_setup(getPCM(), log);
    }

    int dumpHWSetup(){
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
      int ret=0;
      if (!logEnabled())
        return ret;
      return snd_pcm_dump_hw_setup(getPCM(), log);
    }

    int dumpSWSetup(){
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
      int ret=0;
      if (!logEnabled())
        return ret;
      return snd_pcm_dump_sw_setup(getPCM(), log);
    }

    int start(){
      PCM_NOT_OPEN_CHECK_NO_PRINT(getPCM(), int) // check pcm is open
      return snd_pcm_start(getPCM());

    }
    snd_pcm_state_t getState(){
      PCM_NOT_OPEN_CHECK_NO_PRINT(getPCM(), snd_pcm_state_t) // check pcm is open
      return snd_pcm_state(getPCM());
    }

    const char *getStateName(){
        PCM_NOT_OPEN_CHECK_NO_PRINT(getPCM(), char *) // check pcm is open
        return snd_pcm_state_name(getState());
    }
    /** Returns true if in the prepared state
    */
    bool prepared(){
      PCM_NOT_OPEN_CHECK_NO_PRINT(getPCM(), bool) // check pcm is open
      return getState()==SND_PCM_STATE_PREPARED;
    }

    /** Returns true if in the open state
    */
    bool opened(){
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
      return getState()==SND_PCM_STATE_OPEN;
    }

    /** Returns true if in the setup state
    */
    bool isSetup(){
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
      return getState()==SND_PCM_STATE_SETUP;
    }

    /** Returns true if in the running state
    */
    bool running(){
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
      return getState()==SND_PCM_STATE_RUNNING;
    }

    /** Returns true if in the xrun state
    */
    bool hasXrun(){
      PCM_NOT_OPEN_CHECK_NO_PRINT(getPCM(), bool) // check pcm is open
      return getState()==SND_PCM_STATE_XRUN;
    }

    /** Returns true if in the draining state
    */
    bool draining(){
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
      return getState()==SND_PCM_STATE_DRAINING;
    }

    /** Returns true if in the PAUSED state
    */
    bool paused(){
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
      return getState()==SND_PCM_STATE_PAUSED;
    }

    /** Returns true if in the SUSPENDED state
    */
    bool suspended(){
      PCM_NOT_OPEN_CHECK_NO_PRINT(getPCM(), bool) // check pcm is open
      return getState()==SND_PCM_STATE_SUSPENDED;
    }

    /** Returns true if in the disconnected state
    */
    bool disconnected(){
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
      return getState()==SND_PCM_STATE_DISCONNECTED;
    }
  };
}
#endif //PCM_H
