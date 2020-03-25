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
      return snd_pcm_drain(getPCM());
    }

    /** Resets the PCM.
    \return <0 on error
    */
    int reset(){
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
      return snd_pcm_reset(getPCM());
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
        PCM_NOT_OPEN_CHECK_STRING(getPCM()) // check pcm is open
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
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
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
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
      return getState()==SND_PCM_STATE_SUSPENDED;
    }

    /** Returns true if in the disconnected state
    */
    bool disconnected(){
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
      return getState()==SND_PCM_STATE_DISCONNECTED;
    }

    /** Returns the type of the PCM
    \return snd_pcm_type_t the type of the PCM
    */
    snd_pcm_type_t getPCMType(){
      return snd_pcm_type	(getPCM());
    }
  };
}
#endif //PCM_H
